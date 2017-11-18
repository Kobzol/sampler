#pragma once

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
    void connect(const std::vector<std::string>& args) override;

    void stop() override;
    void waitForExit() override;

protected:
    virtual std::unique_ptr<StacktraceCollector> createCollector(uint32_t pid) override;

private:
    void createTasks();
    void loop();

    void stopTasks();

    void handleSignals(TaskContext* context);
    bool checkNewTask(TaskContext* context, int status);

    void initTracee(pid_t pid, bool attached, bool setoptions);

    pid_t pid;

    std::atomic<bool> running{false};
    std::thread loopThread;
};
