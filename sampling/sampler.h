#pragma once

#ifdef __linux__
    #include "unix/sampler.h"
#elif _WIN32
    #include "win32/sampler.h"
#endif
