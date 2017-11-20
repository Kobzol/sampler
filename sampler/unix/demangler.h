#pragma once

#include <unordered_map>
#include <string>

class Demangler
{
public:
    std::string demangle(const std::string& name);

private:
    std::unordered_map<std::string, std::string> names;
};
