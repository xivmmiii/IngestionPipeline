#pragma once
#include <string>
#include <utility>
#include "Severity.hpp"

class LogEntry
{
private:
    Severity severity = Severity::UNKNOWN;
    std::string message;
    long lineNumber = 0;
    std::string timestamp;
    std::string source;

public:
    LogEntry() = default;

    LogEntry(Severity sev, std::string mes, long num, std::string ts, std::string src) : severity(sev), message(std::move(mes)), lineNumber(num), timestamp(std::move(ts)), source(std::move(src)) {}

    const std::string &getTimestamp() const
    {
        return timestamp;
    }

    const std::string &getSource() const
    {
        return source;
    }

    const std::string &getMessage() const
    {
        return message;
    }

    Severity getSeverity() const
    {
        return severity;
    }

    long getLineNumber() const
    {
        return lineNumber;
    }
};
