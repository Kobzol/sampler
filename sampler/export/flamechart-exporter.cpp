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

void FlameChartExporter::exportTask(TaskContext& task, std::ostream& os)
{
    std::unordered_map<std::string, size_t> frameCounter;

    for (auto& trace: task.getCollector().getSamples())
    {
        auto serialized = this->serializeFrame(trace);
        auto it = frameCounter.find(serialized);
        if (it == frameCounter.end())
        {
            it = frameCounter.insert({ serialized, 0 }).first;
        }

        it->second++;
    }

    for (auto& kv: frameCounter)
    {
        os << kv.first + " " + std::to_string(kv.second) << std::endl;
    }
}
