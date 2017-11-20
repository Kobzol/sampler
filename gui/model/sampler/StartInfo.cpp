#include "StartInfo.h"

PidStartInfo::PidStartInfo(uint32_t pid): pid(pid)
{

}
void PidStartInfo::start(Sampler& sampler)
{
    sampler.connect(this->pid);
}

ProgramStartInfo::ProgramStartInfo(std::string program, std::string workingDirectory):
    program(std::move(program)), workingDirectory(std::move(workingDirectory))
{

}
void ProgramStartInfo::start(Sampler& sampler)
{
    sampler.connect(this->program, this->workingDirectory, {}, {});
}
