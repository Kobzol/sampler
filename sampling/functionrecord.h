#pragma once

#include <string>

struct FunctionRecord
{
public:
    explicit FunctionRecord(std::string name);

    std::string name;
    size_t samplesOwn = 0;
    size_t samplesCumulative = 0;
};
