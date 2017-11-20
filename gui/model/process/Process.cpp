#include "Process.h"

Process::Process(uint32_t pid, std::string name): pid(pid), name(std::move(name))
{

}

uint32_t Process::getPid() const
{
    return this->pid;
}

const std::string& Process::getName() const
{
    return this->name;
}
