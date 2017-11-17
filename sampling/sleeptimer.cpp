#include "sleeptimer.h"

#include <unistd.h>

Sleeptimer::Sleeptimer(uint32_t interval): interval(interval)
{

}

void Sleeptimer::mark()
{
    this->timepoint = std::chrono::steady_clock::now();
}

void Sleeptimer::sleep()
{
    auto tp = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - this->timepoint).count();

    if (ms < this->interval)
    {
        usleep(static_cast<unsigned int>(1000 * (this->interval - ms)));
    }
}
