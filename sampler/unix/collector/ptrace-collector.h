#pragma once

#include "../../stacktrace-collector.h"
#include "../resolver/addrline-resolver.h"

class PtraceCollector: public StacktraceCollector
{
public:
    PtraceCollector(uint32_t pid, uint32_t stackLimit, AddrlineResolver& resolver);

    virtual void collect() override;
private:
    AddrlineResolver& resolver;
};
