#pragma once

#include <taskcontext.h>
#include "../../components/common/tree/TreeItem.h"

class TopdownTreeCalculator
{
public:
    struct CallRecord
    {
        std::string parent = "";
        std::string function = "";
        std::string location = "";
        size_t samples = 0;
    };

    void createTopdownTree(TaskContext& context, TreeItem& root,
                           const std::function<std::vector<std::string>(const CallRecord&)>& callback);
};
