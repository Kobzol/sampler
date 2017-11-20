#pragma once

#include <sampler.h>

class StartInfo
{
public:
    StartInfo() = default;
    virtual ~StartInfo() = default;

    virtual void start(Sampler& sampler) = 0;
};

class PidStartInfo: public StartInfo
{
public:
    explicit PidStartInfo(uint32_t pid);

    void start(Sampler& sampler) override;

private:
    uint32_t pid;
};

class ProgramStartInfo: public StartInfo
{
public:
    ProgramStartInfo(std::string program, std::string workingDirectory);

    void start(Sampler& sampler) override;

private:
    std::string program;
    std::string workingDirectory;
};