#include "SamplerManager.h"

#ifdef __linux__
    #include <unix/ptrace-sampler.h>
#endif

SamplerManager::~SamplerManager()
{
    if (this->sampler)
    {
        this->sampler->stop();
    }
}

EventBroadcaster<SamplerManager, SamplingEvent, TaskContext*>& SamplerManager::onSamplerEvent()
{
    return this->samplerEventStream;
}
EventBroadcaster<SamplerManager, const std::exception&>& SamplerManager::onSamplerError()
{
    return this->samplerErrorStream;
}

void SamplerManager::startSampler(uint32_t sampleInterval, std::unique_ptr<StartInfo> info)
{
    this->stopSampler();

#ifdef __linux__
    this->sampler = std::make_unique<PtraceSampler>(sampleInterval);
#endif
    this->sampler->setEventListener([this](SamplingEvent event, TaskContext* context) {
        this->samplerEventStream(event, context);
    });
    this->sampler->setErrorListener([this](const std::exception& error) {
        this->samplerErrorStream(error);
    });
    info->start(*this->sampler);
}

void SamplerManager::stopSampler()
{
    if (this->sampler)
    {
        this->sampler->stop();
    }
}

void SamplerManager::killProcess()
{
    if (this->sampler)
    {
        this->sampler->killProcess();
        this->stopSampler();
    }
}

Sampler* SamplerManager::getSampler()
{
    return this->sampler.get();
}
