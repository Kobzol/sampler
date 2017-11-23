#pragma once

#include <QObject>
#include <memory>
#include <exception>

#include <sampler.h>

#include "../util/event/event-broadcaster.h"
#include "StartInfo.h"

class SamplerManager
{
public:
    SamplerManager() = default;
    ~SamplerManager();

    EventBroadcaster<SamplerManager, SamplingEvent, TaskContext*>& onSamplerEvent();
    EventBroadcaster<SamplerManager, const std::exception&>& onSamplerError();

    void startSampler(uint32_t sampleInterval, std::unique_ptr<StartInfo> info);
    void stopSampler();
    void killProcess();

    Sampler* getSampler();

private:
    EventBroadcaster<SamplerManager, SamplingEvent, TaskContext*> samplerEventStream;
    EventBroadcaster<SamplerManager, const std::exception&> samplerErrorStream;
    std::unique_ptr<Sampler> sampler;
};
