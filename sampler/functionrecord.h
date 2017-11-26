#pragma once

#include <string>

struct FunctionRecord
{
public:
    explicit FunctionRecord(std::string name, std::string location, uint32_t line, void* address);

    const std::string& getName() const;
    const std::string& getLocation() const;
    uint32_t getLine() const;
    void* getAddress() const;

    std::string getFullLocation() const;

private:
    std::string name;
    std::string location;
    uint32_t line;
    void* address;
};
