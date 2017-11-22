#pragma once

#include <string>
#include <vector>

#include <unistd.h>

#define LOG_ERROR(fn)\
    if ((fn) < 0)\
    {\
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));\
    }

std::vector<std::string> getDirectoryFiles(const std::string& directory);
std::string loadExePath(pid_t pid);

bool ends_with(const std::string& value, const std::string& ending);

size_t getTimestamp();
