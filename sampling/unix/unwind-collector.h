#pragma once

#include <unistd.h>
#include <cstdint>
#include <unordered_map>

#include "demangler.h"
#include "../functionrecord.h"
#include "../stacktrace-collector.h"

class UnwindBackstackCollector: public StacktraceCollector
{
public:
    explicit UnwindBackstackCollector(pid_t pid, uint32_t stackLimit);
    ~UnwindBackstackCollector() override;

    void collect() override;

private:
    Demangler demangler;

    void* context = nullptr;
};
