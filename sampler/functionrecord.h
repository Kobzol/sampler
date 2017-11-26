#pragma once

#include <string>

struct FunctionRecord
{
public:
    explicit FunctionRecord(std::string name, std::string location, uint32_t line,
                            std::string module, void* address);

    const std::string& getName() const;
    const std::string& getLocation() const;
    uint32_t getLine() const;
    const std::string& getModule() const;
    void* getAddress() const;

    std::string getFullLocation() const;

private:
    std::string name;
    std::string location;
    uint32_t line;
    std::string module;
    void* address;
};
