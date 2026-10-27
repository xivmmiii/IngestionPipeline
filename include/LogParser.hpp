#pragma once

#include <string>
#include <utility>
#include <sstream>
#include "ParseResult.hpp"
#include "Severity.hpp"
#include "RawLine.hpp"
#include "LogEntry.hpp"

class LogParser
{
public:
    LogParser() = default;

    Severity toSeverity(const std::string &str) const
    {
        if (str == "DEBUG")
            return Severity::DEBUG;
        if (str == "ERROR")
            return Severity::ERROR;
        if (str == "INFO")
            return Severity::INFO;
        if (str == "WARN")
            return Severity::WARN;
        return Severity::UNKNOWN;
    }

    ParseResult parse(const RawLine &raw) const
    {
        const std::string &line = raw.getLine();
        if (line.empty())
        {
            return ParseResult(raw);
        }

        std::istringstream iss(line);
        std::string ts, sevStr, src;

        if (!(iss >> ts >> sevStr >> src))
        {
            return ParseResult(raw);
        }

        Severity sev = toSeverity(sevStr);
        if (sev == Severity::UNKNOWN)
        {
            return ParseResult(raw);
        }

        std::string msg;
        std::getline(iss, msg);

        if (!msg.empty())
        {
            std::size_t first = msg.find_first_not_of(' ');
            if (first != std::string::npos)
            {
                msg = msg.substr(first);
            }
            else
            {
                msg.clear();
            }
        }

        LogEntry entry(sev, msg, raw.getLineNumber(), ts, src);
        bool isDebug = (sev == Severity::DEBUG);
        return ParseResult(entry, raw, isDebug);
    }
};