#pragma once

#include <cstdint>
#include <unordered_map>

#include "functionrecord.h"
#include "stacktrace.h"

class StacktraceCollector
{
public:
    explicit StacktraceCollector(uint32_t pid, uint32_t stackLimit);
    virtual ~StacktraceCollector() = default;

    StacktraceCollector(const StacktraceCollector& other) = delete;
    StacktraceCollector& operator=(const StacktraceCollector& other) = delete;
    StacktraceCollector(const StacktraceCollector&& other) = delete;

    virtual void collect() = 0;

    const std::vector<StackTrace>& getTraces() const;

protected:
    virtual void addTrace(StackTrace trace);

    uint32_t pid;
    uint32_t stackLimit;

    std::vector<StackTrace> traces;
};
