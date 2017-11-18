#include "unwind-collector.h"

#include <libunwind-ptrace.h>

UnwindBackstackCollector::UnwindBackstackCollector(uint32_t pid, uint32_t stackLimit)
        : StacktraceCollector(pi, stackLimit)
{
    this->context = _UPT_create(pid);
}

UnwindBackstackCollector::~UnwindBackstackCollector()
{
    _UPT_destroy(this->context);
}

void UnwindBackstackCollector::collect()
{
    unw_cursor_t cursor{};
    unw_addr_space_t space = unw_create_addr_space(&_UPT_accessors, 0);

    int rc = unw_init_remote(&cursor, space, this->context);

    std::vector<FunctionRecord> records;
    char nameBuffer[512];
    for (int i = 0; i < this->stackLimit; i++)
    {
        unw_word_t  offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        nameBuffer[0] = '\0';
        unw_get_proc_name(&cursor, nameBuffer, sizeof(nameBuffer), &offset);

        auto name = this->demangler.demangle(nameBuffer);
        records.emplace_back(name);

        if (unw_step(&cursor) < 0)
        {
            break;
        }
    }

    this->addTrace(StackTrace(records));
}
