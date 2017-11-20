#pragma once

#include <string>
#include <vector>

#define LOG_ERROR(fn)\
    if ((fn) < 0)\
    {\
        printf("%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));\
    }

std::vector<std::string> getDirectoryFiles(const std::string& directory);
