#pragma once

#include "../taskcontext.h"

class Exporter
{
public:
    virtual ~Exporter() = default;

    virtual void exportTask(TaskContext& task, std::ostream& os) = 0;
};
