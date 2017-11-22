#pragma once

#include <string>

struct FunctionRecord
{
public:
    explicit FunctionRecord(std::string name, std::string location, void* address, size_t timestamp);

    const std::string& getName() const;
    const std::string& getLocation() const;

    void* getAddress() const;
    size_t getTimestamp() const;

private:
    std::string name;
    std::string location;
    void* address;
    size_t timestamp;
};
