#include "functionrecord.h"

FunctionRecord::FunctionRecord(std::string name, std::string location, void* address, size_t timestamp)
        : name(std::move(name)), location(std::move(location)), address(address), timestamp(timestamp)
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

size_t FunctionRecord::getTimestamp() const
{
    return timestamp;
}
