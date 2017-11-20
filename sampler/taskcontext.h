#pragma once

#include <memory>

#include "task.h"
#include "stacktrace-collector.h"

class TaskContext
{
public:
    explicit TaskContext(Task task, std::unique_ptr<StacktraceCollector> collector);

    Task& getTask();
    StacktraceCollector& getCollector();

private:
    Task task;
    std::unique_ptr<StacktraceCollector> collector;
};
