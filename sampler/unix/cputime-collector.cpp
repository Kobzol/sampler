#include "cputime-collector.h"

#include <cassert>
#include <sstream>

CputimeCollector::CputimeCollector()
{
    this->cpustat.open("/cpu/stat");
    assert(this->cpustat.is_open());
}

size_t CputimeCollector::getTotalCpuUsage()
{
    this->cpustat.seekg(0, std::ios::beg);
    std::string line;
    std::getline(this->cpustat, line);

    size_t sum = 0, time;
    std::stringstream ss(line.substr(line.find(' ') + 1));

    // total user, total user low, total sys
    for (int i = 0; i < 3; i++)
    {
        ss >> time;
        sum += time;
    }

    return sum;
}

CputimeCollector::TimeRecord CputimeCollector::getProcessCpuUsage(pid_t pid)
{
    std::ifstream fs("/proc/" + std::to_string(pid) + "/stat");
    if (!fs.is_open()) return {};

    return {};
}
