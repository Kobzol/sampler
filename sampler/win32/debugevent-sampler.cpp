#include "debugevent-sampler.h"
#include "../sleeptimer.h"
#include "stackwalk-collector.h"
#include "utility.h"

#include <iostream>
#include <tlhelp32.h>
#include <DbgHelp.h>
#include <winnt.h>

static std::vector<uint32_t> getThreads(uint32_t pid)
{
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	WRAP_HANDLE(hThreadSnap);

	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return {};
	}

	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &entry))
	{
		CloseHandle(hThreadSnap);
		return {};
	}

	std::vector<uint32_t> tids;
	do
	{
		if (entry.th32OwnerProcessID == pid)
		{
			tids.push_back(entry.th32ThreadID);
		}
	}
	while (Thread32Next(hThreadSnap, &entry));

	CloseHandle(hThreadSnap);
	return tids;
}
static void consumeEvents(std::function<bool (DEBUG_EVENT* event)> callback)
{
	while (true)
	{
		DEBUG_EVENT event{};
		WRAP_ERROR(WaitForDebugEvent(&event, INFINITE));

		if (callback(&event))
		{
			WRAP_ERROR(ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE));
		}
		else break;
	}
}
static void consumeStopEvent(std::function<bool (DEBUG_EVENT* event, std::vector<DWORD>& newThreads)> callback)
{
	std::vector<DWORD> newThreads;
	DWORD stopThread = 0;
	while (true)
	{
		DEBUG_EVENT event{};
		WRAP_ERROR(WaitForDebugEvent(&event, INFINITE));

		if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT && stopThread == 0)
		{
			stopThread = event.dwThreadId;
		}
		else if (event.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT)
		{
			newThreads.push_back(event.dwThreadId);
		}
		else if (event.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT && event.dwThreadId == stopThread)
		{
			WRAP_ERROR(ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE));
			break;
		}

		if (callback(&event, newThreads))
		{
			WRAP_ERROR(ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE));
		}
		else break;
	}
}

DebugeventSampler::DebugeventSampler(uint32_t interval) : Sampler(interval)
{

}

void DebugeventSampler::connect(uint32_t pid)
{
	this->running.store(true);
	this->loopThread = std::thread([this, pid]()
	{
		WRAP_ERROR(DebugActiveProcess(pid));
		WRAP_ERROR(DebugSetProcessKillOnExit(false));

		this->process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		WRAP_HANDLE(this->process);

		consumeStopEvent([this, pid](DEBUG_EVENT* event, std::vector<DWORD>& newThreads)
		{
			if (event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
			{
				this->initializeThreads(event->dwProcessId, event->dwThreadId);
			}
			return true;
		});

		this->loop();
	});
}
void DebugeventSampler::connect(const std::vector<std::string>& args)
{
    std::string cmdline;
    for (size_t i = 1; i <args.size(); i++)
    {
        cmdline += args[i];
        if (i != args.size() - 1)
        {
            cmdline += ' ';
        }
    }

    this->running.store(true);
	this->loopThread = std::thread([this, &args, &cmdline]()
	{
		STARTUPINFO start{};
		start.cb = sizeof(start);

		auto* cmd = new char[cmdline.size() + 1];
		strcpy_s(cmd, cmdline.size() + 1, cmdline.c_str());

		PROCESS_INFORMATION info;
		WRAP_ERROR(CreateProcess(args[0].c_str(), cmd, nullptr, nullptr, false,
			DEBUG_PROCESS, nullptr, nullptr, &start, &info));

        delete[] cmd;

		this->process = info.hProcess;

		consumeEvents([this, &info](DEBUG_EVENT* event)
		{
			if (event->dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
			{
				this->initializeThreads(event->dwProcessId, 0);
				WRAP_ERROR(ContinueDebugEvent(event->dwProcessId, event->dwThreadId, DBG_CONTINUE));
				return false;
			}

			return true;
		});

		this->loop();
    });
}

void DebugeventSampler::stop()
{
	this->running = false;
}

void DebugeventSampler::waitForExit()
{
	this->loopThread.join();
}

void DebugeventSampler::loop()
{
    Sleeptimer timer(this->interval);
    while (this->running)
    {
        timer.mark();

		this->stopTasks();

		consumeStopEvent([this](DEBUG_EVENT* event, std::vector<DWORD>& newThreads)
		{
			return this->handleEvent(event, newThreads);
		});

        timer.sleep();
    }

	this->onEvent(SamplingEvent::Exit, nullptr);
	CloseHandle(this->process);
}

bool DebugeventSampler::handleEvent(DEBUG_EVENT* event, std::vector<DWORD>& newThreads)
{
    if (event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
    {
		this->initializeSymbols();

		for (auto tid : newThreads)
		{
			if (tid != event->dwThreadId)
			{
				this->handleTaskCreate(tid);
			}
		}

		for (int i = 0; i < this->activeTasks.size(); i++)
		{
			this->handleTaskCollect(this->tasks[this->activeTasks[i]].get());
		}
    }
	else if (event->dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT)
	{
		for (int i = 0; i < this->activeTasks.size(); i++)
		{
			int taskIndex = this->activeTasks[i];
			if (this->tasks[taskIndex]->getTask().getPid() == event->dwThreadId)
			{
				this->handleTaskEnd(this->tasks[taskIndex].get(), event->u.ExitThread.dwExitCode);
				this->activeTasks.erase(this->activeTasks.begin() + i);
				break;
			}
		}
	}
	else if (event->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
	{
		this->running = false;
		return false;
	}

	return true;
}

void DebugeventSampler::initializeThreads(DWORD pid, DWORD tidIgnore)
{
	for (auto tid : getThreads(pid))
	{
		if (tid != tidIgnore)
		{
			this->handleTaskCreate(tid);
		}
	}
}
void DebugeventSampler::initializeSymbols()
{
	if (!this->symbolsInitialized)
	{
		this->symbolsInitialized = true;

		SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
		WRAP_ERROR(SymInitialize(this->process, nullptr, TRUE));
	}
}

void DebugeventSampler::stopTasks()
{
	WRAP_ERROR(DebugBreakProcess(this->process));
}

std::unique_ptr<StacktraceCollector> DebugeventSampler::createCollector(uint32_t pid)
{
    return std::make_unique<StackwalkCollector>(this->process, pid, 32);
}
