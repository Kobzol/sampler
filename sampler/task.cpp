#include "task.h"

Task::Task(uint32_t pid): pid(pid)
{

}

uint32_t Task::getPid() const
{
    return this->pid;
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
