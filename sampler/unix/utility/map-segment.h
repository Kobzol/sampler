#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <iostream>

struct MapSegment
{
    MapSegment(size_t start, size_t length, bool readable, bool writable, bool executable, std::string path);

    size_t getStart() const;
    size_t getLength() const;

    bool isReadable() const;
    bool isWritable() const;
    bool isExecutable() const;

    const std::string& getPath() const;

private:
    size_t start;
    size_t length;
    bool readable;
    bool writable;
    bool executable;
    std::string path;
};

std::vector<MapSegment> readSegments(pid_t pid);
