#pragma once

#include <cstdint>

class Task
{
public:
    explicit Task(uint32_t pid);

    uint32_t getPid() const;

    bool isActive() const;
    void deactivate(int exitCode);

    int getExitCode() const;

private:
    bool active = true;
    int exitCode = -1;
    uint32_t pid;
};
