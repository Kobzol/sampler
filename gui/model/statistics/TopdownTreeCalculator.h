#pragma once

#include <taskcontext.h>
#include "../../components/common/tree/TreeItem.h"

class TopdownTreeCalculator
{
public:
    void createTopdownTree(TaskContext& context, TreeItem& root);
};
