#include "ptrace-sampler.h"
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

static std::unique_ptr<char*[]> parseArguments(const std::vector<std::string>& arguments)
{
    auto args = std::make_unique<char*[]>(arguments.size() + 1);
    for (size_t i = 0; i < arguments.size(); i++)
    {
        args[i] = const_cast<char*>(arguments[i].c_str());
    }
    args[arguments.size()] = nullptr;

    return args;
}
static std::unique_ptr<char*[]> parseEnvironment(const std::vector<std::pair<std::string, std::string>>& environment)
{
    auto envp = std::make_unique<char*[]>(environment.size() + 1);
    for (size_t i = 0; i < environment.size(); i++)
    {
        auto* record = new std::string(environment[i].first);
        *record += "=" + environment[i].second;
        envp[i] = const_cast<char*>(record->c_str());
    }
    envp[environment.size()] = nullptr;

    return envp;
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
        this->unwrapLibc(ptrace(PTRACE_ATTACH, pid, nullptr, nullptr),
                         "Couldn't attach to process " + std::to_string(pid));
        this->pid = static_cast<pid_t>(pid);

        this->initTracee(this->pid, true, true);
        this->createTasks();
        this->loop();
    });
}
void PtraceSampler::connect(
        const std::string& program,
        const std::string& cwd,
        const std::vector<std::string>& arguments,
        const std::vector<std::pair<std::string, std::string>>& environment)
{
    assert(!this->running);

    this->running.store(true);
    this->loopThread = std::thread([this, program, cwd, arguments, environment]()
    {
        this->pid = fork();
        this->unwrapLibc(this->pid, "Program start failed");

        if (this->pid == 0)
        {
            auto args = parseArguments(arguments);
            auto envp = parseEnvironment(environment);

            LOG_ERROR(ptrace(PTRACE_TRACEME, 0, nullptr, nullptr));

            if (!cwd.empty())
            {
                LOG_ERROR(chdir(cwd.c_str()));
            }

            LOG_ERROR(execvpe(program.c_str(), args.get(), envp.get()));
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
            this->unwrapLibc(ptrace(PTRACE_ATTACH, task.getPid(), nullptr, nullptr),
                             "Couldn't attach to process thread");
            this->initTracee(task.getPid(), true, true);
        }
    }
}

void PtraceSampler::loop()
{
    this->onEvent(SamplingEvent::Start, nullptr);

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

    this->disconnect();
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
    if (this->running)
    {
        this->running = false;
        this->waitForExit();
    }
    else this->waitForExit();
}

void PtraceSampler::disconnect()
{
    this->stopTasks();
    for (int taskIndex: this->activeTasks)
    {
        auto* task = this->tasks[taskIndex].get();

        int status;
        LOG_ERROR(waitpid(task->getTask().getPid(), &status, WUNTRACED));
        LOG_ERROR(ptrace(PTRACE_DETACH, task->getTask().getPid(), nullptr, 0));
    }
    this->activeTasks.clear();
}

void PtraceSampler::waitForExit()
{
    if (this->loopThread.joinable())
    {
        this->loopThread.join();
    }
}

void PtraceSampler::handleSignals(TaskContext* context)
{
    this->consumeSignals(context->getTask().getPid(), [this, context](int ret, int status)
    {
        if (ret < 0)
        {
#ifdef DEBUG
            std::cerr << "TASKEND waitpid" << context->getTask().getPid() << std::endl;
#endif
            this->handleTaskEnd(context, -1);
            return false;
        }

        if (!this->checkNewTask(context, status))
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

                this->restartRepeatedly(context->getTask().getPid(), context, 0);
                return false;
            }
        }
        else
        {
#ifdef DEBUG
            std::cerr << "HandleTaskCreation for " << context->getTask().getPid() << std::endl;
#endif

            this->restartRepeatedly(context->getTask().getPid(), context, 0);
            return true;
        }

        this->restartRepeatedly(context->getTask().getPid(),
                                context, WIFSIGNALED(status) ? WTERMSIG(status) : WSTOPSIG(status));
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
        this->unwrapLibc(ptrace(PTRACE_GETEVENTMSG, context->getTask().getPid(), nullptr, &pid),
                         "Couldn't attach to process thread");
        
        this->initTracee(pid, true, false);
        return true;
    }
    else return false;
}

void PtraceSampler::initTracee(pid_t pid, bool attached, bool setoptions)
{
    int expectedSignal = attached ? SIGSTOP : SIGTRAP;
    bool success = true;

    this->consumeSignals(pid, [this, pid, expectedSignal, attached, setoptions, &success](int ret, int status)
    {
        if (ret < 0)
        {
            this->unwrapLibc(-1, "Couldn't pause process");
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

                this->unwrapLibc(ptrace(PTRACE_SETOPTIONS, pid, nullptr, options),
                                 "Couldn't initialize thread options");
            }

            if (!this->restartRepeatedly(pid, nullptr, 0))
            {
                success = false;
            }
            return false;
        }
        else if (WIFSTOPPED(status))
        {
            if (!this->restartRepeatedly(pid, nullptr, WSTOPSIG(status)))
            {
                success = false;
                return false;
            }
        }

        return true;
    });

    if (success)
    {
        this->handleTaskCreate(pid);
    }
}

std::unique_ptr<StacktraceCollector> PtraceSampler::createCollector(uint32_t pid)
{
    return std::make_unique<UnwindCollector>(pid, 32);
}

void PtraceSampler::unwrapLibc(long ret, const std::string& message)
{
    if (ret < 0)
    {
        std::string error(!message.empty() ? (message + ": ") : message);
        error += strerror(errno);

        this->onError(std::runtime_error(error));
        this->running = false;
    }
}

bool PtraceSampler::restartRepeatedly(uint32_t pid, TaskContext* task, int signal)
{
    while (true)
    {
#ifdef DEBUG
        std::cerr << "Restarting " << pid << " with signal " << signal << std::endl;
#endif

        if (!this->running) break;
        long ret = ptrace(PTRACE_CONT, pid, nullptr, signal);
#ifdef DEBUG
        std::cerr << "Restarting returned " << ret << std::endl;
#endif
        if (ret < 0)
        {
            if (errno == ESRCH && task != nullptr)
            {
                this->handleTaskEnd(task, -1);
            }
            else this->unwrapLibc(ret, "Couldn't resume thread");

            break;
        }
        else return true;
    }

    return false;
}

void PtraceSampler::consumeSignals(pid_t pid, const std::function<bool(int, int)>& callback, bool checkEnd)
{
    while (true)
    {
#ifdef DEBUG
        std::cerr << "Consuming for " << pid << std::endl;
#endif

        if (checkEnd && !this->running) return;

        // TODO: better solution for timeout
        int status;
        int ret = waitpid(pid, &status, WUNTRACED | WNOHANG);

        if (ret == 0)
        {
            usleep(100);
            continue;
        }

#ifdef DEBUG
        debugStatus(pid, status);
#endif

        if (!callback(ret, status))
        {
            return;
        }
    }
}

void PtraceSampler::killProcess()
{
    this->unwrapLibc(kill(this->pid, SIGKILL), "Couldn't kill process " + std::to_string(this->pid));
}
