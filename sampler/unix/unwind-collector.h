#pragma once

#include <unistd.h>
#include <cstdint>
#include <unordered_map>

#include "demangler.h"
#include "../functionrecord.h"
#include "../stacktrace-collector.h"

class UnwindCollector: public StacktraceCollector
{
public:
    explicit UnwindCollector(uint32_t pid, uint32_t stackLimit);
    ~UnwindCollector() override;

    void collect() override;

private:
    Demangler demangler;

    void* context = nullptr;
};
