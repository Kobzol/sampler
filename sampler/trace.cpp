#include "trace.h"
#include "unix/utility/utility.h"

size_t Trace::getStartTimestamp() const
{
    return this->startTimestamp;
}

size_t Trace::getEndTimestamp() const
{
    return this->endTimestamp;
}

size_t Trace::getDuration() const
{
    return this->getEndTimestamp() - this->getStartTimestamp();
}

void Trace::start()
{
    this->startTimestamp = getTimestamp();
}
void Trace::end()
{
    this->endTimestamp = getTimestamp();
}

void Trace::addTask(std::unique_ptr<TaskContext> task)
{
    this->tasks.push_back(std::move(task));
}

size_t Trace::getTaskCount() const
{
    return this->tasks.size();
}
TaskContext* Trace::getTaskAt(size_t index)
{
    return this->tasks[index].get();
}
