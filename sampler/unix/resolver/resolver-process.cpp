#include "resolver-process.h"
#include "../utility/utility.h"

#include <cstring>
#include <wait.h>
#include <cassert>

ResolverProcess::ResolverProcess(std::string executable, bool library, size_t start, size_t size)
        : executable(executable), library(library), start(start), size(size)
{
    int stdinpipe[2];
    int stdoutpipe[2];

    LOG_ERROR(pipe(stdinpipe));
    LOG_ERROR(pipe(stdoutpipe));

    this->process = fork();
    if (this->process == 0)
    {
        LOG_ERROR(close(stdinpipe[1]));
        LOG_ERROR(close(stdoutpipe[0]));

        LOG_ERROR(dup2(stdinpipe[0], STDIN_FILENO));
        LOG_ERROR(dup2(stdoutpipe[1], STDOUT_FILENO));
        LOG_ERROR(dup2(stdoutpipe[1], STDERR_FILENO));

        std::vector<const char*> args = {
                "addr2line",
                "-C",
                "-e",
                executable.c_str(),
                "-f",
                "-p"
        };

        if (library)
        {
            args.push_back("-j");
            args.push_back(".text");
        }

        args.push_back(nullptr);

        LOG_ERROR(execv("/usr/bin/addr2line", const_cast<char *const *>(args.data())));

        assert(false);
        exit(1);
    }

    LOG_ERROR(close(stdinpipe[0]));
    LOG_ERROR(close(stdoutpipe[1]));

    this->input = stdinpipe[1];
    this->output = stdoutpipe[0];
}

void ResolverProcess::shutdown()
{
    LOG_ERROR(kill(this->process, SIGKILL));
    waitpid(this->process, nullptr, 0);
}

bool ResolverProcess::containsAddress(void* address) const
{
    auto value = reinterpret_cast<size_t>(address);
    return value >= this->start && value < (this->start + this->size);
}

std::string ResolverProcess::resolve(void* address) const
{
    auto value = reinterpret_cast<size_t>(address);
    if (this->library)
    {
        value -= this->start;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%p\n", reinterpret_cast<void*>(value));

    size_t len = strlen(buffer);
    ssize_t count = write(this->input, buffer, len);
    LOG_ERROR(count);

    if (count == len)
    {
        count = read(this->output, buffer, sizeof(buffer));
        LOG_ERROR(count);
        if (count <= 0) return "";

        buffer[count - 1] = '\0';
        return std::string(buffer);
    }
    else return "";
}
