#pragma once

#include <unistd.h>
#include <cstdint>
#include "demangler.h"

class BackstackCollector
{
public:
    explicit BackstackCollector(pid_t pid, uint32_t stackLimit);
    ~BackstackCollector();

    BackstackCollector(const BackstackCollector& other) = delete;
    BackstackCollector operator=(const BackstackCollector& other) = delete;
    BackstackCollector(const BackstackCollector&& other) = delete;

    void collect();

private:
    Demangler demangler;

    void* context = nullptr;
    pid_t pid;
    uint32_t stackLimit;
};
