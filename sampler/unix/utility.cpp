#include "utility.h"
#include <dirent.h>

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
