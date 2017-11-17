#pragma once

#include <string>
#include <vector>

#define WRAP_ERROR(fn)\
    if ((fn) < 0)\
    {\
        printf("%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));\
    }
//throw std::runtime_error(strerror(errno));

std::vector<std::string> getDirectoryFiles(const std::string& directory);
