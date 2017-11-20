#pragma once

#include "Process.h"

#include <vector>

class ProcessLister
{
public:
    std::vector<Process> getRunningProcesses() const;
};
