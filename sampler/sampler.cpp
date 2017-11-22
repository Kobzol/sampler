#include "sampler.h"

Sampler::Sampler(uint32_t interval): interval(interval)
{

}

void Sampler::setEventListener(std::function<void(SamplingEvent, TaskContext*)> listener)
{
    this->onEvent = std::move(listener);
}
void Sampler::setErrorListener(std::function<void(const std::exception&)> listener)
{
    this->onError = std::move(listener);
}

void Sampler::handleTaskCollect(TaskContext* context)
{
    context->getCollector().collect();
    this->onEvent(SamplingEvent::TaskCollect, context);
}

void Sampler::handleTaskEnd(TaskContext* context, int exitCode)
{
    context->getTask().deactivate(exitCode);
    this->onEvent(SamplingEvent::TaskExit, context);
}

TaskContext* Sampler::handleTaskCreate(uint32_t pid)
{
    this->tasks.push_back(std::make_unique<TaskContext>(this->createTask(pid), this->createCollector(pid)));
    this->activeTasks.push_back(static_cast<int>(this->tasks.size() - 1));
    this->onEvent(SamplingEvent::TaskCreation, this->tasks.back().get());

    return this->tasks.back().get();
}
