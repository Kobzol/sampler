#include "ptrace-sampler.h"
#include "utility/utility.h"
#include "../sleeptimer.h"
#include "collector/dw-collector.h"

#include <cstring>
#include <cassert>

#include <sys/ptrace.h>
#include <sys/wait.h>

#ifdef PTRACE_OLDSETOPTIONS
    #undef PTRACE_SETOPTIONS
    #define PTRACE_SETOPTIONS PTRACE_OLDSETOPTIONS
#endif
#define PTRACE_EVENT_STOP (128)

//#define DEBUG

#ifdef DEBUG
static void debugStatus(int pid, int status)
{
    std::cerr << "SIGNAL from " << pid << std::endl;
    std::cerr << "WIFSTOPPED: " << WIFSTOPPED(status) << std::endl;
    std::cerr << "WIFSIGNALED: " << WIFSIGNALED(status) << std::endl;
    std::cerr << "WIFEXITED: " << WIFEXITED(status) << std::endl;
    std::cerr << "WSTOPSIG: " << WSTOPSIG(status) << std::endl;
    std::cerr << "PTRACE_EVENT_STOP: " << ((status >> 16) == PTRACE_EVENT_STOP) << std::endl;
    std::cerr << std::endl;
}
#endif

static std::vector<char*> parseArguments(const std::string& program, const std::vector<std::string>& arguments)
{
    size_t argCount = arguments.size() + 2;
    std::vector<char*> args;
    args.push_back(const_cast<char*>(program.c_str()));

    for (size_t i = 1; i < argCount - 1; i++)
    {
        args.push_back(const_cast<char*>(arguments[i - 1].c_str()));
    }
    args.push_back(nullptr);

    return args;
}
static std::vector<char*> parseEnvironment(const std::vector<std::pair<std::string, std::string>>& environment)
{
    std::vector<char*> envp;
    for (auto& env : environment)
    {
        auto* record = new std::string(env.first);
        *record += "=" + env.second;
        envp.push_back(const_cast<char*>(record->c_str()));
    }
    envp.push_back(nullptr);

    return envp;
}

static long PTRACE_ATTACH_OPTIONS = PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACEEXIT;
static long PTRACE_SPAWN_OPTIONS = PTRACE_ATTACH_OPTIONS | PTRACE_O_EXITKILL;


PtraceSampler::PtraceSampler(uint32_t interval): Sampler(interval)
{

}

void PtraceSampler::attach(uint32_t pid)
{
    assert(!this->running);

    this->running = true;
    this->loopThread = std::thread([this, pid]()
    {
        this->unwrapLibc(ptrace(PTRACE_SEIZE, pid, nullptr, PTRACE_ATTACH_OPTIONS),
                         "Couldn't attach to process " + std::to_string(pid));
        this->pid = static_cast<pid_t>(pid);

        this->initializeProcess(this->pid);
    });
}
void PtraceSampler::spawn(
        const std::string& program,
        const std::string& cwd,
        const std::vector<std::string>& arguments,
        const std::vector<std::pair<std::string, std::string>>& environment)
{
    assert(!this->running);

    this->running = true;
    this->loopThread = std::thread([this, program, cwd, arguments, environment]()
    {
        this->pid = static_cast<uint32_t>(fork());
        this->unwrapLibc(this->pid, "Program start failed");

        if (this->pid == 0)
        {
            auto args = parseArguments(program, arguments);
            auto envp = parseEnvironment(environment);

            if (!cwd.empty())
            {
                LOG_ERROR(chdir(cwd.c_str()));
            }

            LOG_ERROR(execvpe(program.c_str(), args.data(), envp.data()));
            exit(0);
        }

        usleep(1000 * 100);

        this->unwrapLibc(ptrace(PTRACE_SEIZE, this->pid, nullptr, PTRACE_SPAWN_OPTIONS),
                         "Couldn't attach to spawned process");
        this->initializeProcess(this->pid);
    });
}

void PtraceSampler::initializeProcess(uint32_t pid)
{
    this->trace->start();
    this->onEvent(SamplingEvent::Start, nullptr);

    this->handleTaskCreate(pid);
    this->createTasks();
    this->loop();
}

void PtraceSampler::createTasks()
{
    auto tasks = loadExistingTasks(this->pid);
    for (auto& task: tasks)
    {
        // attach to existing threads
        if (task.getPid() != this->pid)
        {
            this->unwrapLibc(ptrace(PTRACE_SEIZE, task.getPid(), nullptr, PTRACE_ATTACH_OPTIONS),
                             "Couldn't attach to process thread " + std::to_string(task.getPid()));
            this->handleTaskCreate(task.getPid());
        }
    }
}

void PtraceSampler::loop()
{
    Sleeptimer timer(this->interval);

    while (this->running && !this->activeTasks[this->activeIndex].empty())
    {
        timer.mark();

        if (!this->paused)
        {
            this->stopTasks();

            auto& currentTasks = this->activeTasks[this->activeIndex];
            auto& nextTasks = this->activeTasks[1 - this->activeIndex];

            size_t count = currentTasks.size();
            for (int i = 0; i < count; i++)
            {
                int taskIndex = currentTasks[i];
                auto* task = this->trace->getTaskAt(static_cast<size_t>(taskIndex));

                if (task->getTask().isActive())
                {
                    int status;
                    int ret = waitpid(task->getTask().getPid(), &status, WUNTRACED);
                    if (ret < 0)
                    {
                        this->handleTaskEnd(task, -1);
                    }
                    else if (this->checkStopSignal(task, status))
                    {
                        nextTasks.push_back(taskIndex);
                    }
                }
            }

            // add newly created tasks
            size_t newCount = currentTasks.size();
            for (size_t i = count; i < newCount; i++)
            {
                nextTasks.push_back(currentTasks[i]);
            }

            currentTasks.clear();
            this->activeIndex = 1 - this->activeIndex;
        }

        timer.sleep();
    }

    this->disconnect();

    this->trace->end();
    this->onEvent(SamplingEvent::Exit, nullptr);
}

void PtraceSampler::stopTask(TaskContext& task)
{
    long ret = ptrace(PTRACE_INTERRUPT, task.getTask().getPid(), nullptr, nullptr);
    LOG_ERROR(ret);

    if (ret < 0 && errno == ESRCH)
    {
        this->handleTaskEnd(&task, -1);
    }
}
void PtraceSampler::stopTasks()
{
    for (int taskIndex: this->activeTasks[this->activeIndex])
    {
        auto* task = this->trace->getTaskAt(static_cast<size_t>(taskIndex));
        this->stopTask(*task);
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

    for (int taskIndex: this->activeTasks[this->activeIndex])
    {
        auto* task = this->trace->getTaskAt(static_cast<size_t>(taskIndex));
        if (task->getTask().isActive())
        {
            int status;
            int ret = waitpid(task->getTask().getPid(), &status, WUNTRACED);
            if (ret > 0)
            {
                LOG_ERROR(ptrace(PTRACE_DETACH, task->getTask().getPid(), nullptr, 0));
            }
        }
    }

    this->activeTasks[this->activeIndex].clear();

    this->resume();
}

void PtraceSampler::waitForExit()
{
    if (this->loopThread.joinable())
    {
        this->loopThread.join();
    }
}

bool PtraceSampler::checkStopSignal(TaskContext* context, int status)
{
    // new task created
    if (this->checkNewTask(context, status))
    {
        this->restartRepeatedly(context->getTask().getPid(), context, 0);
        return true;
    }

    if (WIFEXITED(status))
    {
        this->handleTaskEnd(context, static_cast<uint32_t>(WEXITSTATUS(status)));
        return false;
    }

    if (WIFSIGNALED(status))
    {
        this->handleTaskEnd(context, static_cast<uint32_t>(WTERMSIG(status)));
        return false;
    }

    if (WIFSTOPPED(status) && ((status >> 16) == PTRACE_EVENT_STOP)) // expected stop
    {
        this->handleTaskCollect(context);
        this->restartRepeatedly(context->getTask().getPid(), context, 0);
        return true;
    }

    this->restartRepeatedly(context->getTask().getPid(),
                            context, WIFSIGNALED(status) ? WTERMSIG(status) : WSTOPSIG(status));
    return true;
}
bool PtraceSampler::checkNewTask(TaskContext* context, int status)
{
    if (((status >> 8) == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)))  ||
        ((status >> 8) == (SIGTRAP | (PTRACE_EVENT_FORK << 8)))   ||
        ((status >> 8) == (SIGTRAP | (PTRACE_EVENT_VFORK << 8))))
    {
        unsigned long pid;
        this->unwrapLibc(ptrace(PTRACE_GETEVENTMSG, context->getTask().getPid(), nullptr, &pid),
                         "Couldn't attach to new process thread");

        this->handleTaskCreate(pid);
        return true;
    }
    else return false;
}

std::unique_ptr<StacktraceCollector> PtraceSampler::createCollector(uint32_t pid)
{
    return std::make_unique<DWCollector>(pid, 32);
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
        if (!this->running) break;
        long ret = ptrace(PTRACE_CONT, pid, nullptr, signal);

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

void PtraceSampler::killProcess()
{
    this->unwrapLibc(kill(this->pid, SIGKILL), "Couldn't kill process " + std::to_string(this->pid));
}

std::vector<Task> PtraceSampler::loadExistingTasks(pid_t pid)
{
    std::string taskPath = "/proc/" + std::to_string(pid) + "/task";
    auto tids = getDirectoryFiles(taskPath);

    std::vector<Task> tasks;
    for (auto& tid : tids)
    {
        auto threadid = static_cast<uint32_t>(std::stoul(tid, nullptr, 10));
        tasks.push_back(this->createTask(threadid));
    }
    return tasks;
}

Task PtraceSampler::createTask(uint32_t pid)
{
    return Task(pid, loadThreadName(pid));
}
