#include "stacktrace-collector.h"

StacktraceCollector::StacktraceCollector(uint32_t pid, uint32_t stackLimit): pid(pid), stackLimit(stackLimit)
{

}

const std::vector<StackTrace>& StacktraceCollector::getSamples() const
{
    return this->traces;
}
void StacktraceCollector::addTrace(StackTrace trace)
{
    this->traces.push_back(std::move(trace));
}
