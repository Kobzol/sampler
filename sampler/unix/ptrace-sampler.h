#pragma once

#include <iostream>

#include "../sampler.h"
#include "../sampling-event.h"
#include "../task.h"
#include "../taskcontext.h"
#include "unwind-collector.h"

class PtraceSampler: public Sampler
{
public:
    explicit PtraceSampler(uint32_t interval);

    void connect(uint32_t pid) override;
    void connect(const std::string& program,
                 const std::string& cwd,
                 const std::vector<std::string>& arguments,
                 const std::vector<std::pair<std::string, std::string>>& environment) override;

    void stop() override;
    void waitForExit() override;
    void killProcess() override;

protected:
    virtual std::unique_ptr<StacktraceCollector> createCollector(uint32_t pid) override;

private:
    void createTasks();
    void stopTasks();
    void loop();
    void disconnect();

    bool checkStopSignal(TaskContext* context, int status);
    bool checkNewTask(TaskContext* context, int status);

    void initTracee(pid_t pid, bool attached, bool setoptions);
    bool restartRepeatedly(uint32_t pid, TaskContext* task, int signal);
    void consumeSignals(pid_t pid, const std::function<bool (int, int)>& callback,
                        bool checkEnd=true);

    void unwrapLibc(long ret, const std::string& message = "");

    pid_t pid;

    std::atomic<bool> running{false};
    std::thread loopThread;
};
