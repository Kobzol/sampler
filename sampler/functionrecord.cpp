#include "functionrecord.h"

FunctionRecord::FunctionRecord(std::string name, std::string location, void* address)
        : name(std::move(name)), location(std::move(location)), address(address)
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

void* FunctionRecord::getAddress() const
{
    return address;
}
