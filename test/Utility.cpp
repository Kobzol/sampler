#include "Utility.h"
#include "../sampler/unix/ptrace-sampler.h"

#include <cassert>

#include <unistd.h>
#include <cstring>
#include <fstream>
#include <catch.hpp>

uint32_t createProcess(const std::string& program, int& out, int& in)
{
    // 0 - read, 1 - write
    int inPipe[2];
    int outPipe[2];
    pipe(inPipe);
    pipe(outPipe);

    std::string path = "./program/test-" + program;

    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0)
    {
        close(inPipe[1]);
        close(outPipe[0]);

        dup2(outPipe[1], STDOUT_FILENO);
        dup2(outPipe[1], STDERR_FILENO);
        dup2(inPipe[0], STDIN_FILENO);

        execl(path.c_str(), "", nullptr);
        assert(false);
        exit(1);
    }
    else
    {
        close(inPipe[0]);
        close(outPipe[1]);

        out = outPipe[0];
        in = inPipe[1];
    }

    return static_cast<uint32_t>(pid);
}

std::unique_ptr<Sampler> createSampler(uint32_t frequency, bool catchErrors)
{
    auto sampler = std::make_unique<PtraceSampler>(frequency);

    if (catchErrors)
    {
        sampler->setErrorListener([](const std::exception& error) {
            WARN(error.what());
        });
    }

    return std::move(sampler);
}
