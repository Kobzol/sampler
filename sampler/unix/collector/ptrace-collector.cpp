#include "ptrace-collector.h"
#include "../utility/utility.h"
#include <cstring>
#include <algorithm>
#include <sys/ptrace.h>
#include <sys/user.h>

PtraceCollector::PtraceCollector(uint32_t pid, uint32_t stackLimit, AddrlineResolver& resolver)
        : StacktraceCollector(pid, stackLimit), resolver(resolver)
{

}

void PtraceCollector::collect()
{
    user_regs_struct regs{};
    LOG_ERROR(ptrace(PTRACE_GETREGS, this->pid, 0, &regs));
    size_t rbp = regs.rbp;
    size_t rip = regs.rip;
    size_t timestamp = getTimestamp();

    std::vector<FunctionRecord> records;
    while (rbp != 0 && rbp != (size_t) -1)
    {
        auto addr = (void*) rip;
        auto location = this->resolver.resolve(addr);
        records.emplace_back(location, location, addr);

        errno = 0;
        size_t result = ptrace(PTRACE_PEEKTEXT, this->pid, rbp + 8, 0);
        if (result == (size_t) -1 && errno != 0)
        {
            break;
        }
        rip = result;

        errno = 0;
        result = ptrace(PTRACE_PEEKTEXT, this->pid, rbp, 0);
        if (result == (size_t) -1 && errno != 0)
        {
            break;
        }

        rbp = result;
    }

    std::reverse(records.begin(), records.end());
    this->addTrace(StackTrace(records, timestamp));
}
