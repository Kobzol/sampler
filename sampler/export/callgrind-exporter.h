#pragma once

#include "exporter.h"

#include <ostream>

class CallgrindExporter: public Exporter
{
public:
    virtual void exportTask(TaskContext& task, std::ostream& os) override;
};
