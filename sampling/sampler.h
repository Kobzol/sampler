#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <thread>

#include "taskcontext.h"
#include "sampling-event.h"

class Sampler
{
public:
    explicit Sampler(uint32_t interval);
    virtual ~Sampler() = default;

    virtual void connect(uint32_t pid) = 0;
    virtual void connect(const std::vector<std::string>& args) = 0;

    virtual void stop() = 0;
    virtual void waitForExit() = 0;

    virtual void setEventListener(std::function<void(SamplingEvent, TaskContext*)> listener);

protected:
    virtual void handleTaskEnd(TaskContext* context, int exitCode);
    virtual void handleTaskCollect(TaskContext* context);
    virtual TaskContext* handleTaskCreate(uint32_t pid);
    virtual std::unique_ptr<StacktraceCollector> createCollector(uint32_t pid) = 0;

    uint32_t interval;
    std::vector<std::unique_ptr<TaskContext>> tasks;
    std::vector<int> activeTasks;

    std::function<void(SamplingEvent, TaskContext*)> onEvent;
};
