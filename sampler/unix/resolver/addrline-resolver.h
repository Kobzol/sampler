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
    ~AddrlineResolver();

    AddrlineResolver(const AddrlineResolver& other) = delete;
    AddrlineResolver operator=(const AddrlineResolver& other) = delete;
    AddrlineResolver(const AddrlineResolver&& other) = delete;

    std::string resolve(void* address);

private:
    std::string getFromProcesses(void* address);

    void loadResolvers(pid_t pid);

    std::vector<std::unique_ptr<ResolverProcess>> processes;
    std::unordered_map<void*, std::string> lineCache;
};
