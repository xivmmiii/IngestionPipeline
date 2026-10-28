#pragma once
#include "BoundedQueue.hpp"
#include "RawLine.hpp"
#include "WriterManager.hpp"
#include "Stats.hpp"
#include "LogParser.hpp"
#include "ParseResult.hpp"

class Worker
{
private:
    BoundedQueue<RawLine> &queue;
    Stats &stats;
    WriterManager &writer;
    LogParser parser;

public:
    Worker(BoundedQueue<RawLine> &q, WriterManager &w, Stats &s)
        : queue(q), stats(s), writer(w), parser() {}

    void run()
    {
        RawLine raw;
        while (queue.pop(raw))
        {
            stats.linesRead.fetch_add(1);

            ParseResult result = parser.parse(raw);

            if (result.isMalformed)
            {
                writer.writeMalformed(result.raw);
                stats.malformed.fetch_add(1);
            }
            else if (result.isDebug)
            {
                // DEBUG is filtered - not written to output
                stats.filtered.fetch_add(1);
            }
            else
            {
                writer.writeSeverity(result.entry, result.raw.getLine());
                stats.written.fetch_add(1);
            }
        }
    }
};