#pragma once

#include <string>

class ResolverProcess
{
public:
    explicit ResolverProcess(std::string executable, bool library, size_t start, size_t length);

    ResolverProcess(const ResolverProcess& other) = delete;
    ResolverProcess operator=(const ResolverProcess& other) = delete;
    ResolverProcess(const ResolverProcess&& other) = delete;

    std::string resolve(void* address) const;
    bool containsAddress(void* address) const;
    void shutdown();

//private:
    std::string executable;
    bool library;
    size_t start;
    size_t size;

    pid_t process;
    int input;
    int output;
};
