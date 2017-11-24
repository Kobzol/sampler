#include "StartInfo.h"

PidStartInfo::PidStartInfo(uint32_t pid): pid(pid)
{

}
void PidStartInfo::start(Sampler& sampler)
{
    sampler.attach(this->pid);
}

ProgramStartInfo::ProgramStartInfo(std::string program, std::string workingDirectory,
                                   std::vector<std::string> arguments, std::vector<std::string> environment):
    program(std::move(program)), workingDirectory(std::move(workingDirectory)),
    arguments(std::move(arguments)), environment(std::move(environment))
{

}
void ProgramStartInfo::start(Sampler& sampler)
{
    std::vector<std::pair<std::string, std::string>> environment;
    for (auto& env : this->environment)
    {
        auto it = env.find('=');
        if (it == std::string::npos)
        {
            environment.emplace_back(env, "");
        }
        else environment.emplace_back(env.substr(0, it), env.substr(it + 1));
    }

    sampler.spawn(this->program, this->workingDirectory, this->arguments, environment);
}
