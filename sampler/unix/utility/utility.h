#pragma once

#include <string>
#include <vector>

#include <unistd.h>

#define LOG_ERROR(fn)\
    if ((fn) < 0)\
    {\
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));\
    }

#define PROFILE_START(name)\
    size_t name = getTimestamp()

#define PROFILE_END(name)\
    std::cerr << getTimestamp() - (name) << " ms" << std::endl;

std::vector<std::string> getDirectoryFiles(const std::string& directory);
std::string loadExePath(pid_t pid);
std::string loadThreadName(pid_t pid);

bool ends_with(const std::string& value, const std::string& ending);

size_t getTimestamp();
