#include <iostream>
#include <algorithm>
#include <cstring>
#include "dw-collector.h"
#include "../utility/utility.h"

#define DWFL_CHECK(ret, success)\
    if ((ret) != (success))\
    {\
        std::cerr << dwfl_errmsg(dwfl_errno()) << std::endl;\
    }

DWCollector::DWCollector(uint32_t pid, uint32_t stackLimit)
        : StacktraceCollector(pid, stackLimit), frameContext(std::make_unique<FrameContext>())
{
    this->offline_callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
    this->offline_callbacks.find_elf = dwfl_linux_proc_find_elf;

    this->dwfl = dwfl_begin(&this->offline_callbacks);
    DWFL_CHECK(dwfl_linux_proc_report(this->dwfl, this->pid), 0);
}

int DWCollector::handleFrame(Dwfl_Frame* frame, void* arg)
{
    auto* frameContext = static_cast<DWCollector::FrameContext*>(arg);

    Dwarf_Addr pc;
    bool isactivation;

    if (frame != nullptr)
    {
        bool success = dwfl_frame_pc(frame, &pc, &isactivation);
        if (!success || !pc) return DWARF_CB_ABORT;
        if (!isactivation)
        {
            pc--;
        }

        Dwfl_Thread* thread = dwfl_frame_thread(frame);
        Dwfl* dwfl = dwfl_thread_dwfl(thread);
        Dwfl_Module* module = dwfl_addrmodule(dwfl, pc);

        std::string name, location;
        if (module)
        {
            const char* resolved = dwfl_module_addrname(module, pc);
            if (resolved != nullptr)
            {
                name = frameContext->demangler.demangle(resolved);

                Dwfl_Line* line = dwfl_module_getsrc(module, pc);
                if (line != nullptr)
                {
                    int linenum = -1;
                    const char* file = dwfl_lineinfo(line, nullptr, &linenum, nullptr, nullptr, nullptr);

                    if (file)
                    {
                        location = std::string(file) + ":" + std::to_string(linenum);
                    }
                }
            }
        }

        frameContext->frames.emplace_back(name, location, (void*) pc);
    }
    else return DWARF_CB_ABORT;

    return DWARF_CB_OK;
}

void DWCollector::collect()
{
    if (!this->attached)
    {
        DWFL_CHECK(dwfl_linux_proc_attach(this->dwfl, this->pid, true), 0);
        this->attached = true;
    }

    size_t timestamp = getTimestamp();
    this->frameContext->frames.clear();
    DWFL_CHECK(dwfl_getthread_frames(this->dwfl, this->pid, handleFrame, this->frameContext.get()), 0);

    std::reverse(this->frameContext->frames.begin(), this->frameContext->frames.end());
    this->addTrace(StackTrace(this->frameContext->frames, timestamp));
}
