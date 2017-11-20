#include "sleeptimer.h"
#ifdef __linux__
	#include <unistd.h>
#elif _WIN32
	#include <Windows.h>
#endif

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
		size_t timeLeft = this->interval - ms;
#ifdef __linux__
        usleep(static_cast<unsigned int>(1000 * timeLeft));
#elif _WIN32
		Sleep(timeLeft);
#endif
    }
}
