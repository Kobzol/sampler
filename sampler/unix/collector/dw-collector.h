#pragma once

#include <elfutils/libdwfl.h>
#include "../../stacktrace-collector.h"
#include "../resolver/addrline-resolver.h"
#include "../demangler.h"

class DWCollector: public StacktraceCollector
{
    struct FrameContext
    {
        Demangler demangler;
        std::vector<FunctionRecord> frames;
    };

public:
    DWCollector(uint32_t pid, uint32_t stackLimit);

    virtual void collect() override;

private:
    static int handleFrame(Dwfl_Frame* frame, void* arg);

    Dwfl_Callbacks offline_callbacks{};
    Dwfl* dwfl;
    bool attached = false;

    std::unique_ptr<FrameContext> frameContext;
};
