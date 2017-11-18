#include "PtraceSampler.h"
#include "utility.h"
#include "unwind-collector.h"
#include "../taskcontext.h"
#include "../sleeptimer.h"

#include <cstring>
#include <iostream>
#include <cassert>

#include <sys/ptrace.h>
#include <sys/wait.h>

#include <asm/unistd.h>
#define tgkill(gtid, tid, sig) syscall (__NR_tgkill, (gtid), (tid), (sig))

#ifdef PTRACE_OLDSETOPTIONS
    #undef PTRACE_SETOPTIONS
    #define PTRACE_SETOPTIONS PTRACE_OLDSETOPTIONS
#endif

//#define DEBUG

#ifdef DEBUG
static void debugStatus(int pid, int status)
{
    std::cerr << "SIGNAL from " << pid << std::endl;
    std::cerr << "WIFSTOPPED: " << WIFSTOPPED(status) << std::endl;
    std::cerr << "WIFSIGNALED: " << WIFSIGNALED(status) << std::endl;
    std::cerr << "WIFEXITED: " << WIFEXITED(status) << std::endl;
    std::cerr << "WSTOPSIG: " << WSTOPSIG(status) << std::endl;
    std::cerr << std::endl;
}
#endif

static void restartRepeatedly(int pid, int signal)
{
    while (true)
    {
#ifdef DEBUG
        std::cerr << "Restarting " << pid << " with signal " << signal << std::endl;
#endif

        long ret = ptrace(PTRACE_CONT, pid, nullptr, signal);
#ifdef DEBUG
        std::cerr << "Restarting returned " << ret << std::endl;
#endif
        if (ret < 0 && (errno == EINTR || errno == ESRCH))
        {
            if (errno == ESRCH) std::cerr << "ESRCH" << std::endl;

            continue;
        }
        else break;
    }
}

static std::vector<Task> loadExistingTasks(pid_t pid)
{
    std::string taskPath = "/proc/" + std::to_string(pid) + "/task";
    auto tids = getDirectoryFiles(taskPath);

    std::vector<Task> tasks;
    for (auto& tid : tids)
    {
        tasks.emplace_back(static_cast<uint32_t>(std::stoul(tid, nullptr, 10)));
    }
    return tasks;
}

static void consumeSignals(pid_t pid, const std::function<bool (int, int)>& callback)
{
    while (true)
    {
#ifdef DEBUG
        std::cerr << "Consuming for " << pid << std::endl;
#endif

        int status;
        int ret = waitpid(pid, &status, WUNTRACED);

#ifdef DEBUG
        debugStatus(pid, status);
#endif

        if (!callback(ret, status))
        {
            return;
        }
    }
}


PtraceSampler::PtraceSampler(uint32_t interval): Sampler(interval)
{

}

void PtraceSampler::connect(uint32_t pid)
{
    assert(!this->running);

    this->running = true;
    this->loopThread = std::thread([this, pid]()
    {
        WRAP_ERROR(ptrace(PTRACE_ATTACH, pid, nullptr, nullptr));
        this->pid = static_cast<pid_t>(pid);

        this->initTracee(this->pid, true, true);
        this->createTasks();
        this->loop();
    });
}
void PtraceSampler::connect(const std::vector<std::string>& args)
{
    assert(!this->running);

    this->running.store(true);
    this->loopThread = std::thread([this, &args]()
    {
        this->pid = fork();
        WRAP_ERROR(this->pid);

        if (this->pid == 0)
        {
            auto cargs = std::make_unique<char*[]>(args.size());
            for (int i = 1; i < args.size(); i++)
            {
                cargs[i - 1] = const_cast<char*>(args[i].c_str());
            }
            cargs[args.size() - 1] = nullptr;

            WRAP_ERROR(ptrace(PTRACE_TRACEME, 0, nullptr, nullptr));
            WRAP_ERROR(execv(args[0].c_str(), cargs.get()));
            exit(0);
        }

        this->initTracee(this->pid, false, true);
        this->createTasks();
        this->loop();
    });
}

void PtraceSampler::createTasks()
{
    auto tasks = loadExistingTasks(this->pid);
    for (auto& task: tasks)
    {
        // attach to existing threads
        if (task.getPid() != this->pid)
        {
            ptrace(PTRACE_ATTACH, task.getPid(), nullptr, nullptr);
            this->initTracee(task.getPid(), true, true);
        }
    }
}

void PtraceSampler::loop()
{
    Sleeptimer timer(this->interval);
    while (this->running && !this->activeTasks.empty())
    {
#ifdef DEBUG
        std::cerr << "Loop start: " << this->activeTasks.size() << " active threads" << std::endl;
#endif

        timer.mark();
        this->stopTasks();

        std::vector<int> nextTasks;
        size_t count = this->activeTasks.size();
        for (int i = 0; i < count; i++)
        {
            int taskIndex = this->activeTasks[i];
            auto* task = this->tasks[taskIndex].get();

            if (task->getTask().isActive())
            {
                this->handleSignals(task);

                if (task->getTask().isActive())
                {
                    nextTasks.push_back(taskIndex);
                }
            }
        }

        // add newly created tasks
        for (size_t i = count; i < this->activeTasks.size(); i++)
        {
            nextTasks.push_back(this->activeTasks[i]);
        }

        this->activeTasks = std::move(nextTasks);

#ifdef DEBUG
        std::cerr << "Sleeping" << std::endl;
#endif
        timer.sleep();
    }

    this->onEvent(SamplingEvent::Exit, nullptr);
}

void PtraceSampler::stopTasks()
{
    for (int taskIndex: this->activeTasks)
    {
        auto* task = this->tasks[taskIndex].get();
        if (tgkill(this->pid, task->getTask().getPid(), SIGSTOP) < 0 && errno == ESRCH)
        {
            this->handleTaskEnd(task, -1);
        }
    }
}

void PtraceSampler::stop()
{
    this->running = false;
    this->waitForExit();

    this->stopTasks();
    for (int taskIndex: this->activeTasks)
    {
        auto* task = this->tasks[taskIndex].get();
        consumeSignals(task->getTask().getPid(), [task](int ret, int status)
        {
            if (ret < 0) return false;
            if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSTOP)
            {
                ptrace(PTRACE_DETACH, task->getTask().getPid(), nullptr, nullptr);
                return false;
            }

            return true;
        });
    }
}

void PtraceSampler::waitForExit()
{
    this->loopThread.join();
}

void PtraceSampler::handleSignals(TaskContext* context)
{
    consumeSignals(context->getTask().getPid(), [this, context](int ret, int status)
    {
        if (ret < 0)
        {
#ifdef DEBUG
            std::cerr << "TASKEND waitpid" << context->getTask().getPid() << std::endl;
#endif
            this->handleTaskEnd(context, -1);
            return false;
        }

        if (!this->handleTaskCreation(context, status))
        {
            if (WIFEXITED(status)) // exit
            {
#ifdef DEBUG
                std::cerr << "WIFEXITED " << context->getTask().getPid() << std::endl;
#endif
                this->handleTaskEnd(context, static_cast<uint32_t>(WEXITSTATUS(status)));
                return false;
            }
            else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSTOP) // expected stop
            {
#ifdef DEBUG
                std::cerr << "EXPECTED SIGSTOP " << context->getTask().getPid() << std::endl;
#endif
                this->handleTaskCollect(context);

                restartRepeatedly(context->getTask().getPid(), 0);
                return false;
            }
        }
        else
        {
#ifdef DEBUG
            std::cerr << "HandleTaskCreation for " << context->getTask().getPid() << std::endl;
#endif

            restartRepeatedly(context->getTask().getPid(), 0);
            return true;
        }

        restartRepeatedly(context->getTask().getPid(), WSTOPSIG(status));
        return true;
    });
}
bool PtraceSampler::checkNewTask(TaskContext* context, int status)
{
    if (((status >> 8) == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)))  ||
        ((status >> 8) == (SIGTRAP | (PTRACE_EVENT_FORK << 8)))   ||
        ((status >> 8) == (SIGTRAP | (PTRACE_EVENT_VFORK << 8))))
    {
        unsigned long pid;
        WRAP_ERROR(ptrace(PTRACE_GETEVENTMSG, context->getTask().getPid(), nullptr, &pid));
        
        this->initTracee(pid, true, false);
        return true;
    }
    else return false;
}

void PtraceSampler::initTracee(pid_t pid, bool attached, bool setoptions)
{
    int expectedSignal = attached ? SIGSTOP : SIGTRAP;
    consumeSignals(pid, [pid, expectedSignal, attached, setoptions](int ret, int status)
    {
        if (ret < 0)
        {
            assert(false);
            return false;
        }

        if (WIFSTOPPED(status) && WSTOPSIG(status) == expectedSignal)
        {
            if (setoptions)
            {
                int options = PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEEXIT;
                if (!attached)
                {
                    options |= PTRACE_O_EXITKILL;
                }

                WRAP_ERROR(ptrace(PTRACE_SETOPTIONS, pid, nullptr, options));
            }

            restartRepeatedly(pid, 0);
            return false;
        }
        else if (WIFSTOPPED(status))
        {
            restartRepeatedly(pid, WSTOPSIG(status));
        }

        return true;
    });

    this->handleTaskCreation(pid);
}

std::unique_ptr<BackstackCollector> createCollector(uint32_t pid)
{
    return std::make_unique<UnwindCollector>(pid, 32);
}
