#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>

#include "backstack-collector.h"
#include "../samplingEvent.h"
#include "../task.h"
#include "../taskcontext.h"

class Sampler
{
public:
    explicit Sampler(uint32_t interval);

    void connect(uint32_t pid);
    void connect(const std::vector<std::string>& args);

    void stop();
    void waitForExit();

    void setEventListener(std::function<void(SamplingEvent, TaskContext*)> listener);

private:
    void createTasks();
    void loop();

    void stopTasks();

    void handleSignals(TaskContext* context);
    bool handleTaskCreation(TaskContext* context, int status);
    void handleTaskEnd(TaskContext* context, int exitCode);
    void handleTaskCollect(TaskContext* context);

    void initTracee(pid_t pid, bool attached, bool setoptions);

    uint32_t interval;
    pid_t pid;
    std::vector<std::unique_ptr<TaskContext>> tasks;
    std::vector<int> activeTasks;

    std::atomic<bool> running{false};
    std::thread loopThread;
    std::function<void(SamplingEvent, TaskContext*)> onEvent;
};
