#include "unwind-collector.h"
#include "utility/utility.h"

#include <libunwind-ptrace.h>
#include <iostream>
#include <algorithm>

UnwindCollector::UnwindCollector(uint32_t pid, uint32_t stackLimit, AddrlineResolver& resolver)
        : StacktraceCollector(pid, stackLimit), resolver(resolver)
{
    this->context = _UPT_create(pid);
    this->space = unw_create_addr_space(&_UPT_accessors, 0);
    unw_set_caching_policy(this->space, UNW_CACHE_PER_THREAD);
}

UnwindCollector::~UnwindCollector()
{
    _UPT_destroy(this->context);
    unw_destroy_addr_space(this->space);
}

void UnwindCollector::collect()
{
    unw_cursor_t cursor{};

    int rc = unw_init_remote(&cursor, this->space, this->context);
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
            records.emplace_back(name, location, (void*) pc);
        }

        if (unw_step(&cursor) < 0)
        {
            break;
        }
    }

    std::reverse(records.begin(), records.end());
    this->addTrace(StackTrace(records, timestamp));
}
