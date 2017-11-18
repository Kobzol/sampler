#pragma once

#include <unistd.h>
#include <cstdint>
#include <unordered_map>

#include "demangler.h"
#include "../functionrecord.h"

class BackstackCollector
{
public:
    explicit BackstackCollector(pid_t pid, uint32_t stackLimit);
    ~BackstackCollector();

    BackstackCollector(const BackstackCollector& other) = delete;
    BackstackCollector operator=(const BackstackCollector& other) = delete;
    BackstackCollector(const BackstackCollector&& other) = delete;

    void collect();

    const std::unordered_map<std::string, FunctionRecord> getFunctions() const;

private:
    void addSample(const std::string& name, bool own);

    Demangler demangler;
    std::unordered_map<std::string, FunctionRecord> functions;

    void* context = nullptr;
    pid_t pid;
    uint32_t stackLimit;
};
