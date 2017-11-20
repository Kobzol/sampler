#pragma once

#include <cstdint>
#include <string>

struct Process
{
public:
    Process() = default;
    Process(uint32_t pid, std::string name);

    uint32_t getPid() const;
    const std::string& getName() const;

private:
    uint32_t pid = 0;
    std::string name = "";
};
