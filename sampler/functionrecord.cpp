#include "functionrecord.h"

FunctionRecord::FunctionRecord(std::string name, std::string location, uint32_t line,
                               std::string module, void* address)
        : name(std::move(name)), location(std::move(location)), line(line), module(std::move(module)), address(address)
{

}

const std::string& FunctionRecord::getName() const
{
    return name;
}

const std::string& FunctionRecord::getLocation() const
{
    return location;
}

uint32_t FunctionRecord::getLine() const
{
    return this->line;
}

void* FunctionRecord::getAddress() const
{
    return address;
}

std::string FunctionRecord::getFullLocation() const
{
    return this->location + ":" + std::to_string(this->line);
}

const std::string& FunctionRecord::getModule() const
{
    return this->module;
}
