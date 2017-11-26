#include "TopdownTreeCalculator.h"

TopdownTreeCalculator::CallRecord::CallRecord(std::string parent, std::string function,
                                              std::string location, std::string module)
        : parent(std::move(parent)), function(std::move(function)),
          location(std::move(location)), module(std::move(module))
{

}

const std::string& TopdownTreeCalculator::CallRecord::getParent() const
{
    return this->parent;
}

const std::string& TopdownTreeCalculator::CallRecord::getFunction() const
{
    return this->function;
}

const std::string& TopdownTreeCalculator::CallRecord::getLocation() const
{
    return this->location;
}

const std::string& TopdownTreeCalculator::CallRecord::getModule() const
{
    return this->module;
}

size_t TopdownTreeCalculator::CallRecord::getSamples() const
{
    return this->samples;
}

size_t TopdownTreeCalculator::CallRecord::getOwnSamples() const
{
    return this->ownSamples;
}

void TopdownTreeCalculator::CallRecord::addSample(bool own)
{
    this->samples++;
    if (own) this->ownSamples++;
}

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
                    it = counts.insert({ hash, CallRecord(parent, function, functions[stackLevel].getFullLocation(),
                                                         functions[stackLevel].getModule())
                    }).first;
                }

                it->second.addSample(stackLevel == functions.size() - 1);
            }
        }

        if (counts.empty()) break;

        std::vector<CallRecord> orderedCounts;
        orderedCounts.reserve(counts.size());
        for (auto& kv : counts)
        {
            orderedCounts.push_back(kv.second);
        }

        std::sort(orderedCounts.begin(), orderedCounts.end(), [](const CallRecord& p1, const CallRecord& p2) {
            return p1.getSamples() >= p2.getSamples();
        });

        std::unordered_map<std::string, TreeItem*> nextParents;
        for (auto& record: orderedCounts)
        {
            auto* item = new TreeItem(callback(record));
            parents[record.getParent()]->addChild(item);
            nextParents.insert({ record.getFunction(), item });
        }

        parents = std::move(nextParents);
        stackLevel++;
    }
}
