#pragma once

#include <cstdint>
#include <chrono>

class Sleeptimer
{
public:
    explicit Sleeptimer(uint32_t interval);

    void mark();
    void sleep();

private:
    uint32_t interval;
    std::chrono::steady_clock::time_point timepoint;
};
