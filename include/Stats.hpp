#pragma once
#include <atomic>
//makes a variable safe to read/write from multiple threads without mutex
// has .load() to read
// has .store(val) to write
// has .fetch_add(n) to add n
// has .fetch_sub(n) to minus n

class Stats
{
public:
    std::atomic<long> linesRead{0};
    std::atomic<long> filtered{0};
    std::atomic<long> malformed{0};
    std::atomic<long> written{0};

    // Checks no line is lost or double-counted
    // 'invariant' is standard term for assertion that must always hold
    bool invariant() const
    {
        return linesRead.load() == (filtered.load() + malformed.load() + written.load());
    }

    //to reset the values held in the members
    void reset(){
        linesRead = 0;
        malformed = 0;
        written = 0;
        filtered = 0;
    }
};