#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>

#include "taskcontext.h"
#include "sampling-event.h"
#include "trace.h"

class Sampler
{
public:
    explicit Sampler(uint32_t interval);
    virtual ~Sampler() = default;

    virtual void attach(uint32_t pid) = 0;
    virtual void spawn(const std::string& program,
                       const std::string& cwd,
                       const std::vector<std::string>& args,
                       const std::vector<std::pair<std::string, std::string>>& environment) = 0;

    virtual void stop() = 0;
    virtual void waitForExit() = 0;
    virtual void killProcess() = 0;

    virtual void setEventListener(std::function<void(SamplingEvent, TaskContext*)> listener);
    virtual void setErrorListener(std::function<void(const std::exception&)> listener);

    void pause();
    void resume();
    bool isPaused();

    Trace* getTrace();

protected:
    virtual void handleTaskEnd(TaskContext* context, int exitCode);
    virtual void handleTaskCollect(TaskContext* context);
    virtual TaskContext* handleTaskCreate(uint32_t pid);

    virtual std::unique_ptr<StacktraceCollector> createCollector(uint32_t pid) = 0;
    virtual Task createTask(uint32_t pid) = 0;

    uint32_t interval;
    std::unique_ptr<Trace> trace;
    std::vector<int> activeTasks;
    std::atomic<bool> paused { false };

    std::function<void(SamplingEvent, TaskContext*)> onEvent = [](SamplingEvent, TaskContext*){ };
    std::function<void(const std::exception& error)> onError = [](const std::exception&){ };
};
