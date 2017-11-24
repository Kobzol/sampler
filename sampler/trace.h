#pragma once

#include "taskcontext.h"

class Trace
{
public:
    void start();
    void end();

    void addTask(std::unique_ptr<TaskContext> task);

    size_t getTaskCount() const;
    TaskContext* getTaskAt(size_t index);

    size_t getStartTimestamp() const;
    size_t getEndTimestamp() const;
    size_t getDuration() const;

private:
    size_t startTimestamp = 0;
    size_t endTimestamp = 0;
    std::vector<std::unique_ptr<TaskContext>> tasks;
};
