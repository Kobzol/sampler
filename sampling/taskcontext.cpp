#include "taskcontext.h"

TaskContext::TaskContext(Task task, std::unique_ptr<BackstackCollector> collector)
        : task(task), collector(std::move(collector))
{

}

Task& TaskContext::getTask()
{
    return this->task;
}

BackstackCollector& TaskContext::getCollector()
{
    return *this->collector;
}
