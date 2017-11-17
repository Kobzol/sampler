#pragma once

#include <memory>

#include "task.h"
#include "backstack-collector.h"

class TaskContext
{
public:
    explicit TaskContext(Task task, std::unique_ptr<BackstackCollector> collector);

    Task& getTask();
    BackstackCollector& getCollector();

private:
    Task task;
    std::unique_ptr<BackstackCollector> collector;
};
