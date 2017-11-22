#pragma once

#include <unistd.h>
#include <cstdint>
#include <unordered_map>

#include "demangler.h"
#include "../functionrecord.h"
#include "../stacktrace-collector.h"
#include "resolver/addrline-resolver.h"

class UnwindCollector: public StacktraceCollector
{
public:
    explicit UnwindCollector(uint32_t pid, uint32_t stackLimit, AddrlineResolver& resolver);
    ~UnwindCollector() override;

    void collect() override;

private:
    Demangler demangler;
    AddrlineResolver& resolver;

    void* context = nullptr;
};
