#include "sampler.h"

Sampler::Sampler(uint32_t interval): interval(interval), trace(std::make_unique<Trace>())
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
    this->trace->addTask(std::make_unique<TaskContext>(this->createTask(pid), this->createCollector(pid)));
    this->activeTasks[this->activeIndex].push_back(static_cast<int>(this->trace->getTaskCount() - 1));

    auto* task = this->trace->getTaskAt(this->trace->getTaskCount() - 1);
    this->onEvent(SamplingEvent::TaskCreate, task);

    return task;
}

void Sampler::pause()
{
    if (!this->paused)
    {
        this->paused = true;
        this->onEvent(SamplingEvent::Pause, nullptr);
    }
}
void Sampler::resume()
{
    if (this->paused)
    {
        this->paused = false;
        this->onEvent(SamplingEvent::Resume, nullptr);
    }
}

bool Sampler::isPaused()
{
    return this->paused;
}

Trace* Sampler::getTrace()
{
    return this->trace.get();
}
