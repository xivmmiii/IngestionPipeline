#pragma once
#include <string>
#include <filesystem>
#include <thread>
#include <vector>
#include <stdexcept>
#include "BoundedQueue.hpp"
#include "RawLine.hpp"
#include "Worker.hpp"
#include "LogReader.hpp"
#include "Stats.hpp"
#include "WriterManager.hpp"

class LogProcessor
{
public:
    void process(const std::string &inputFile, int N, size_t capacity, const std::string &outDir)
    {
        // initialise queue, reader and writer manager
        BoundedQueue<RawLine> queue(capacity);
        Stats stats;
        WriterManager writerManager(outDir);
        LogReader reader(inputFile, queue, stats);

        // starts 1 thread
        // & captures everything by reference
        std::thread readerThread([&]
                                 { reader.run(); });

        // initialise worker vector to store N worker threads
        std::vector<std::thread> workerThreads;
        std::vector<Worker> workers;

        // reserve space for N workers
        // If we don't reserve, vector reallocates and moves Workers in memory while threads are using them — crash.
        workers.reserve(N);

        // emplace_back = we give constructor arguments, it builds Worker directly inside vector, no copy.
        // it is faster

        //  creates N worker objects
        for (int i = 0; i < N; ++i)
            workers.emplace_back(queue, writerManager, stats);

        // i signifies that thread 0 gets assigned to worker 0
        // i by value = each thread gets its own copy of i at that moment. Thread 0 gets copy 0, runs workers[0].run()
        for (int i = 0; i < N; ++i)
            workerThreads.emplace_back([&, i]
                                       { workers[i].run(); });
        // join() is for wait, it waits for threads to finish
        // blocks main thread until reader finishes and then the workers finish
        readerThread.join();

        for (auto &t : workerThreads)
            t.join();

        writerManager.closeAll();

        if (!stats.invariant())
            throw std::runtime_error("Stats invariant failed");
    }
};
