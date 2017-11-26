#pragma once

#include <taskcontext.h>
#include "../../components/common/tree/TreeItem.h"

class TopdownTreeCalculator
{
public:
    struct CallRecord
    {
        CallRecord(std::string parent, std::string function,
                   std::string location, std::string module);

        const std::string& getParent() const;
        const std::string& getFunction() const;
        const std::string& getLocation() const;
        const std::string& getModule() const;

        size_t getSamples() const;
        size_t getOwnSamples() const;

        void addSample(bool own);

    private:
        std::string parent;
        std::string function;
        std::string location;
        std::string module;
        size_t samples = 0;
        size_t ownSamples = 0;
    };

    void createTopdownTree(TaskContext& context, TreeItem& root,
                           const std::function<std::vector<std::string>(const CallRecord&)>& callback);
};
