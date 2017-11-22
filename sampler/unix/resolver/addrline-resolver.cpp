#include "addrline-resolver.h"
#include "../utility/utility.h"
#include "../utility/map-segment.h"

#include <csignal>

AddrlineResolver::AddrlineResolver(pid_t pid)
{
    this->loadResolvers(pid);
}

void AddrlineResolver::shutdown()
{
    for (auto& process: this->processes)
    {
        process->shutdown();
    }
}

std::string AddrlineResolver::resolve(void* address)
{
    auto it = this->lineCache.find(address);
    if (it == this->lineCache.end())
    {
        it = this->lineCache.insert({ address, this->getFromProcesses(address) }).first;
    }

    return it->second;
}

std::string AddrlineResolver::getFromProcesses(void* address)
{
    for (auto& process: this->processes)
    {
        if (process->containsAddress(address))
        {
            return process->resolve(address);
        }
    }

    return "";
}

void AddrlineResolver::loadResolvers(pid_t pid)
{
    auto exe = loadExePath(pid);
    auto segments = readSegments(pid);

    for (auto& segment: segments)
    {
        if (segment.isExecutable() && !segment.getPath().empty())
        {
            this->processes.push_back(std::make_unique<ResolverProcess>(segment.getPath(),
                                                                        ends_with(segment.getPath(), ".so"),
                                                                        segment.getStart(),
                                                                        segment.getLength()));
        }
    }
}
