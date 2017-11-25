#include "TopdownTreeCalculator.h"

static std::string hashRecord(const std::string& parent, const std::string& function)
{
    return parent + "-" + function;
}

void TopdownTreeCalculator::createTopdownTree(TaskContext& context, TreeItem& root,
                                              const std::function<std::vector<std::string>(const CallRecord&)>&
                                              callback)
{
    auto& traces = context.getCollector().getSamples();

    std::unordered_map<std::string, TreeItem*> parents = {
            {"", &root}
    };
    int stackLevel = 0;
    while (true)
    {
        std::unordered_map<std::string, CallRecord> counts;
        for (size_t i = 0; i < traces.size(); i++)
        {
            auto& functions = traces[i].getFunctions();
            if (stackLevel < functions.size())
            {
                std::string parent = stackLevel == 0 ? "" : functions[stackLevel - 1].getName();
                std::string function = functions[stackLevel].getName();
                std::string hash = hashRecord(parent, function);

                auto it = counts.find(hash);
                if (it == counts.end())
                {
                    it = counts.insert({hash, CallRecord {
                            parent, function, functions[stackLevel].getLocation(), 0
                    } }).first;
                }

                it->second.samples++;
            }
        }

        if (counts.empty()) break;

        std::vector<CallRecord> orderedCounts(counts.size());
        for (auto& kv : counts)
        {
            orderedCounts.push_back(kv.second);
        }

        std::sort(orderedCounts.begin(), orderedCounts.end(), [](const CallRecord& p1, const CallRecord& p2) {
            return p1.samples >= p2.samples;
        });

        std::unordered_map<std::string, TreeItem*> nextParents;
        for (auto& record: orderedCounts)
        {
            auto* item = new TreeItem(callback(record));
            parents[record.parent]->addChild(item);
            nextParents.insert({ record.function, item });
        }

        parents = std::move(nextParents);
        stackLevel++;
    }
}
