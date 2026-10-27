#pragma once

#include <utility>
#include "LogEntry.hpp"
#include "RawLine.hpp"

class ParseResult
{
public:
    bool isMalformed = false;
    bool isDebug = false;
    LogEntry entry;
    RawLine raw;

    ParseResult() = default;

    // If Worker, LogProcessor, or WriterManager accidentally writes return raw;
    // or push(raw) where ParseResult is expected, without explicit the compiler would silently accept it and create a malformed result.With explicit, it throws compile error at that exact line
    explicit ParseResult(const RawLine &r)
        : isMalformed(true), isDebug(false), raw(r) {}

    // valid
    ParseResult(LogEntry e, RawLine r, bool debug)
        : isMalformed(false), isDebug(debug), entry(std::move(e)), raw(std::move(r)) {}
    
    bool isValid() const
    {
        return !isMalformed && !isDebug;
    }

    bool shouldFilter() const
    {
        return isDebug;
    }

    const LogEntry &getEntry() const
    {
        return entry;
    }

    const RawLine &getRaw() const
    {
        return raw;
    }

    void reset()
    {
        isMalformed = false;
        isDebug = false;
        entry = LogEntry{};
        raw = RawLine{};
    }
};