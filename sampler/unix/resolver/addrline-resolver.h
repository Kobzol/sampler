#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include <unistd.h>
#include "resolver-process.h"

class AddrlineResolver
{
public:
    explicit AddrlineResolver(pid_t pid);

    std::string resolve(void* address);

    void shutdown();

private:
    std::string getFromProcesses(void* address);

    void loadResolvers(pid_t pid);

    std::vector<std::unique_ptr<ResolverProcess>> processes;
    std::unordered_map<void*, std::string> lineCache;
};
