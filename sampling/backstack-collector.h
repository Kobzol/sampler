#pragma once

#ifdef __linux__
    #include "unix/backstack-collector.h"
#elif _WIN32
    #include "win32/backstack-collector.h"
#endif
