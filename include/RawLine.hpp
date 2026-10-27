#pragma once
#include <string>
#include <utility>

class RawLine
{
private:
    long lineNumber = 0;
    std::string line;

public:
    // std::move so that line moves and not gets copied
    // faster and memory-efficient
    RawLine(long num, std::string log) : lineNumber(num), line(std::move(log)) {}
    RawLine();
    const std::string &getLine() const
    {
        return line;
    }
    long getLineNumber() const
    {
        return lineNumber;
    }
};