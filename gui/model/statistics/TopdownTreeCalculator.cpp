#include "TopdownTreeCalculator.h"

struct CallRecord
{
    std::string parent = "";
    std::string function = "";
    size_t samples = 0;
};

static std::string hashRecord(const std::string& parent, const std::string& function)
{
    return parent + "-" + function;
}

void TopdownTreeCalculator::createTopdownTree(TaskContext& context, TreeItem& root)
{
    auto& traces = context.getCollector().getTraces();

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
                    it = counts.insert({hash, CallRecord { parent, function, 0 } }).first;
                }

                it->second.samples++;
            }
        }

        if (counts.empty()) break;

        std::unordered_map<std::string, TreeItem*> nextParents;
        for (auto& kv: counts)
        {
            std::vector<std::string> columns = { kv.second.function, std::to_string(kv.second.samples) };
            auto* item = new TreeItem(columns);
            parents[kv.second.parent]->addChild(item);
            nextParents.insert({ kv.second.function, item });
        }

        parents = std::move(nextParents);
        stackLevel++;
    }
}
