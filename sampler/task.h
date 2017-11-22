#pragma once

#include <cstdint>
#include <string>

class Task
{
public:
    explicit Task(uint32_t pid, std::string name);

    uint32_t getPid() const;
    std::string getName() const;

    bool isActive() const;
    void deactivate(int exitCode);

    int getExitCode() const;

private:
    uint32_t pid;
    std::string name;

    bool active = true;
    int exitCode = -1;
};
