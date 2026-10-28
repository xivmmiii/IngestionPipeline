#pragma once
#include <string>
#include <fstream>
#include "BoundedQueue.hpp"
#include "RawLine.hpp"
#include "Stats.hpp"

class LogReader
{
private:
    std::string filePath;
    BoundedQueue<RawLine> &queue;
    Stats &stats;

public:
    LogReader(const std::string &inputFile, BoundedQueue<RawLine> &q, Stats &s)
        : filePath(inputFile), queue(q), stats(s) {}

    void run()
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            queue.close();
            return;
        }

        std::string line;
        long lineNum = 1;

        while (std::getline(file, line))
        {
            RawLine raw(lineNum, line);
            bool pushed = queue.push(raw);
            if (!pushed)
            {
                break;
            }
            lineNum++;
        }

        queue.close();
    }
};