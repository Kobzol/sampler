#include "map-segment.h"

MapSegment::MapSegment(size_t start, size_t length, bool readable, bool writable, bool executable, std::string path)
        : start(start), length(length), readable(readable), writable(writable),
          executable(executable), path(std::move(path))
{

}

size_t MapSegment::getStart() const
{
    return start;
}

size_t MapSegment::getLength() const
{
    return length;
}

bool MapSegment::isReadable() const
{
    return readable;
}

bool MapSegment::isWritable() const
{
    return writable;
}

bool MapSegment::isExecutable() const
{
    return executable;
}

const std::string& MapSegment::getPath() const
{
    return path;
}

std::vector<MapSegment> readSegments(pid_t pid)
{
    std::vector<MapSegment> segments;
    std::ifstream fs("/proc/" + std::to_string(pid) + "/maps");
    std::string line;
    std::smatch match;
    std::regex regex("([a-fA-F0-9]+)-([a-fA-F0-9]+) ([r-])([w-])([x-]).*");

    if (!fs.is_open())
    {
        return {};
    }

    while (std::getline(fs, line))
    {
        if (std::regex_match(line, match, regex))
        {
            std::string start = match[1];
            std::string end = match[2];
            std::string readable = match[3];
            std::string writable = match[4];
            std::string executable = match[5];
            std::string path;

            size_t it = line.find_first_of('/');
            if (it != std::string::npos)
            {
                path = line.substr(it);
            }

            size_t pos;
            size_t base = std::stoul(start, &pos, 16);
            size_t baseEnd = std::stoul(end, &pos, 16);
            segments.emplace_back(base, baseEnd - base, readable == "r", writable == "w", executable == "x", path);
        }
        else std::cerr << "Couldn't parse map line " + line << std::endl;
    }

    return segments;
}
