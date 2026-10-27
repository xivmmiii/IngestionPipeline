#pragma once
#include "BoundedQueue.hpp"
#include "RawLine.hpp"
#include "WriterManager.hpp"
#include "Stats.hpp"
#include "LogParser.hpp"
#include "LogEntry.hpp"

class Worker
{
private:
    BoundedQueue<RawLine> &queue;
    Stats &stats;
    WriterManager &writer;
    LogParser parser;

public:
    Worker(BoundedQueue<RawLine> &q, WriterManager &w, Stats &s) : queue(q), stats(s), writer(w) {}

    void run()
    {
        // create a rawline object to store popped log
        RawLine raw;
        // pop the log and parse it
        while (queue.pop(raw))
        {
            // update stats
            stats.linesRead.fetch_add(1);
            // create logentry obj
            LogEntry entry = parser.parse(raw);
            // check severity
            if (entry.malformed)
            {
                // update stats
                stats.malformed.fetch_add(1);
                continue;
            }
            // add logentry to the file
            bool didWrite = writer.write(entry);
            // update stats
            if (didWrite)
                stats.written.fetch_add(1);
            else
                stats.filtered.fetch_add(1);
        }
    }
};
