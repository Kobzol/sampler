#include "utility.h"
#include <dirent.h>
#include <cstring>
#include <chrono>

std::vector<std::string> getDirectoryFiles(const std::string& directory)
{
    std::vector<std::string> paths;

    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) return paths;

    while (true)
    {
        dirent* entry = readdir(dir);
        if (entry == nullptr) break;

        if (entry->d_name[0] != '.')
        {
            paths.emplace_back(entry->d_name);
        }
    }
    closedir(dir);
}

std::string loadExePath(pid_t pid)
{
    char buffer[512];
    std::string taskPath = "/proc/" + std::to_string(pid) + "/exe";
    ssize_t len = readlink(taskPath.c_str(), buffer, sizeof(buffer));
    LOG_ERROR(len);

    buffer[len] = '\0';

    return std::string(buffer);
}

bool ends_with(const std::string& value, const std::string& ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

size_t getTimestamp()
{
    return static_cast<size_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()
    );
}
