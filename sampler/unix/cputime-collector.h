#pragma once

#include <fstream>

class CputimeCollector
{
    struct TimeRecord
    {
    public:
        size_t user   = 0;
        size_t system = 0;
    };

public:
    CputimeCollector();

private:
    size_t getTotalCpuUsage();
    TimeRecord getProcessCpuUsage(pid_t pid);

    std::ifstream cpustat;
};
