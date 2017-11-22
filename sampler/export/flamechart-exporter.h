#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../sampler.h"

class FlameChartExporter
{
public:
    std::vector<std::string> createCondensedFrames(TaskContext& task);

private:
    std::string serializeFrame(const StackTrace& trace);

    std::unordered_map<std::string, size_t> frameCounter;
};
