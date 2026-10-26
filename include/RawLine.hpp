#pragma once
#include <string>

class RawLine
{
private:
    long lineNumber;
    std::string line;

public:
    // std::move so that line moves and not gets copied
    // faster and memory-efficient
    RawLine(long num, std::string log) : lineNumber(num), line(std::move(log)) {}
};