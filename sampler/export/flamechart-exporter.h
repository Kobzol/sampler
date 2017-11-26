#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../sampler.h"
#include "exporter.h"

class FlameChartExporter: public Exporter
{
public:
    virtual void exportTask(TaskContext& task, std::ostream& os) override;

private:
    std::string serializeFrame(const StackTrace& trace);
};
