#include "functionrecord.h"

FunctionRecord::FunctionRecord(std::string name, std::string location, uint32_t line, void* address)
        : name(std::move(name)), location(std::move(location)), line(line), address(address)
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
