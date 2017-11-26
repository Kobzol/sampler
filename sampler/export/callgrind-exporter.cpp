#include "callgrind-exporter.h"

struct CallRecord
{
    size_t ownSamples = 0;
    std::string name;
    std::string location;

    std::unordered_map<std::string, size_t> calls;
};

static std::string hash(const FunctionRecord& record)
{
    return record.getLocation() + "-" + record.getName();
}

void CallgrindExporter::exportTask(TaskContext& task, std::ostream& os)
{
    os << "# callgrind format" << std::endl;
    os << "events: samples" << std::endl << std::endl;

    std::unordered_map<std::string, CallRecord> records;

    int stackLevel = 0;
    while (true)
    {
        bool stackLevelPresent = false;

        for (auto& sample: task.getCollector().getSamples())
        {
            auto& functions = sample.getFunctions();
            if (stackLevel < functions.size())
            {
                stackLevelPresent = true;

                auto fnHash = hash(functions[stackLevel]);
                auto it = records.find(fnHash);
                if (records.find(fnHash) == records.end())
                {
                    it = records.insert({ fnHash,
                                          CallRecord { 0,
                                                       functions[stackLevel].getName(),
                                                       functions[stackLevel].getLocation() }
                                        }).first;
                }

                if (stackLevel == functions.size() - 1)
                {
                    it->second.ownSamples++;
                }
                if (stackLevel > 0)
                {
                    auto parentHash = hash(functions[stackLevel - 1]);
                    records[parentHash].calls[fnHash]++;
                }
            }
        }

        if (!stackLevelPresent) break;
        stackLevel++;
    }

    for (auto& record: records)
    {
        os << "fl=" << record.second.location << std::endl;
        os << "fn=" << record.second.name << std::endl;
        os << "1 " << std::max(record.second.ownSamples, 1UL) << std::endl;
        for (auto& calledHash: record.second.calls)
        {
            auto& calledRecord = records[calledHash.first];
            os << "cfi=" << calledRecord.location << std::endl;
            os << "cfn=" << calledRecord.name << std::endl;
            os << "calls=" << calledHash.second << " 1" << std::endl;
            os << "1 1" << std::endl;
        }
    }
}
