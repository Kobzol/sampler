#pragma once

#include <string>
#include <vector>

#include "functionrecord.h"

class StackTrace
{
public:
    explicit StackTrace(std::vector<FunctionRecord> functions, size_t timestamp);

    const std::vector<FunctionRecord>& getFunctions() const;
    size_t getTimestamp() const;

private:
    std::vector<FunctionRecord> functions;
    size_t timestamp;
};
