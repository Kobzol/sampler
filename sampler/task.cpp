#include "task.h"

Task::Task(uint32_t pid, std::string name): pid(pid), name(std::move(name))
{

}

uint32_t Task::getPid() const
{
    return this->pid;
}
std::string Task::getName() const
{
    return this->name;
}

bool Task::isActive() const
{
    return this->active;
}
void Task::deactivate(int exitCode)
{
    this->active = false;
    this->exitCode = exitCode;
}

int Task::getExitCode() const
{
    return this->exitCode;
}
