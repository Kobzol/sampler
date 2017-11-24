#pragma once

#include <string>

struct FunctionRecord
{
public:
    explicit FunctionRecord(std::string name, std::string location, void* address);

    const std::string& getName() const;
    const std::string& getLocation() const;

    void* getAddress() const;

private:
    std::string name;
    std::string location;
    void* address;
};
