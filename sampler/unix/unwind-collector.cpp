#include "unwind-collector.h"
#include "utility/utility.h"

#include <libunwind-ptrace.h>
#include <iostream>

UnwindCollector::UnwindCollector(uint32_t pid, uint32_t stackLimit, AddrlineResolver& resolver)
        : StacktraceCollector(pid, stackLimit), resolver(resolver)
{
    this->context = _UPT_create(pid);
}

UnwindCollector::~UnwindCollector()
{
    _UPT_destroy(this->context);
}

void UnwindCollector::collect()
{
    unw_cursor_t cursor{};
    unw_addr_space_t space = unw_create_addr_space(&_UPT_accessors, 0);

    int rc = unw_init_remote(&cursor, space, this->context);
    size_t timestamp = getTimestamp();

    std::vector<FunctionRecord> records;
    char nameBuffer[512];
    for (int i = 0; i < this->stackLimit; i++)
    {
        unw_word_t  offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);

        if (pc != 0)
        {
            nameBuffer[0] = '\0';
            unw_get_proc_name(&cursor, nameBuffer, sizeof(nameBuffer), &offset);

            auto location = this->resolver.resolve((void*) pc);
            auto name = this->demangler.demangle(nameBuffer);
            records.emplace_back(name, location, (void*) pc, timestamp);
        }

        if (unw_step(&cursor) < 0)
        {
            break;
        }
    }

    this->addTrace(StackTrace(records));
}
