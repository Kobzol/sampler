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

        std::unordered_map<std::string, TreeItem*> nextParents;
        for (auto& kv: counts)
        {
            auto* item = new TreeItem(callback(kv.second));
            parents[kv.second.parent]->addChild(item);
            nextParents.insert({ kv.second.function, item });
        }

        parents = std::move(nextParents);
        stackLevel++;
    }
}
