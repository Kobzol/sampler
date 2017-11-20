#include "stackwalk-collector.h"

#include "utility.h"

#include <Windows.h>
#include <iostream>
#include <DbgHelp.h>

StackwalkCollector::StackwalkCollector(HANDLE process, uint32_t pid, uint32_t stackLimit) : StacktraceCollector(pid, stackLimit), process(process)
{
    
}

void StackwalkCollector::collect()
{
    HANDLE thread = OpenThread(THREAD_ALL_ACCESS, false, this->pid);
	WRAP_HANDLE(thread);

    CONTEXT ctx{};
    ctx.ContextFlags = CONTEXT_ALL;
	WRAP_ERROR(GetThreadContext(thread, &ctx));

    STACKFRAME64 frame{};
    frame.AddrPC.Offset = ctx.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = ctx.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = ctx.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;

    std::vector<FunctionRecord> records;
    while (true)
    {
        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, this->process, thread, &frame, &ctx, nullptr,
                         SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
        {
            break;
        }

        auto* symbol = (IMAGEHLP_SYMBOL64*)malloc(sizeof(IMAGEHLP_SYMBOL64) + 255 * sizeof(TCHAR));
        memset(symbol, 0, sizeof(IMAGEHLP_SYMBOL64) + 255 * sizeof(TCHAR));
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        symbol->MaxNameLength = 255;

        DWORD64 displacement = 0;
        int ret = SymGetSymFromAddr64(this->process, (ULONG64)frame.AddrPC.Offset, &displacement, symbol);
		if (ret != ERROR_INVALID_ADDRESS)
		{
			char name[256];
			WRAP_ERROR(UnDecorateSymbolName(symbol->Name, (PSTR)name, 256, UNDNAME_COMPLETE));

            records.emplace_back(name);
		}

        free(symbol);
    }

    this->addTrace(StackTrace(records));

    WRAP_ERROR(CloseHandle(thread));
}
