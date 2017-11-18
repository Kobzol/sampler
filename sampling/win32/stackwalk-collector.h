#include "../stacktrace-collector.h"

#include <Windows.h>

class StackwalkCollector: public StacktraceCollector
{
public:
    StackwalkCollector(HANDLE process, uint32_t pid, uint32_t stackLimit);

    void collect() override;

private:
    HANDLE process = nullptr;
};
