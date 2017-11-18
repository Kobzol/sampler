#pragma once

#include <string>
#include <vector>

#include "functionrecord.h"

class StackTrace
{
public:
    explicit StackTrace(std::vector<FunctionRecord> functions);

    const std::vector<FunctionRecord>& getFunctions() const;

private:
    std::vector<FunctionRecord> functions;
};
