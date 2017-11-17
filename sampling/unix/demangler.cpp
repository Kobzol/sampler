#include "demangler.h"

#include <cxxabi.h>

std::string Demangler::demangle(const std::string& name)
{
    auto it = this->names.find(name);
    if (it != this->names.end()) return it->second;

    int status;
    char* demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);

    std::string result(name);
    if (status == 0 && demangled) // demangled successfully
    {
        result = demangled;
        free(demangled);
    }

    this->names.insert({name, result});

    return result;
}
