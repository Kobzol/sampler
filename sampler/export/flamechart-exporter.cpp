#include "flamechart-exporter.h"
#include <sstream>

std::string FlameChartExporter::serializeFrame(const StackTrace& trace)
{
    auto functions = trace.getFunctions();

    std::stringstream ss;
    for (ssize_t i = 0; i < functions.size(); i++)
    {
        ss << functions[i].getName();
        if (i != functions.size() - 1) ss << ";";
    }
    return ss.str();
}

std::vector<std::string> FlameChartExporter::createCondensedFrames(TaskContext& task)
{
    std::vector<std::string> frames;

    for (auto& trace: task.getCollector().getTraces())
    {
        auto serialized = this->serializeFrame(trace);
        auto it = this->frameCounter.find(serialized);
        if (it == this->frameCounter.end())
        {
            it = this->frameCounter.insert({ serialized, 0 }).first;
        }

        it->second++;
    }

    for (auto& kv: this->frameCounter)
    {
        frames.push_back(kv.first + " " + std::to_string(kv.second));
    }

    return frames;
}
