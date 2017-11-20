#include "taskcontext.h"

TaskContext::TaskContext(Task task, std::unique_ptr<StacktraceCollector> collector)
        : task(task), collector(std::move(collector))
{

}

Task& TaskContext::getTask()
{
    return this->task;
}

StacktraceCollector& TaskContext::getCollector()
{
    return *this->collector;
}
