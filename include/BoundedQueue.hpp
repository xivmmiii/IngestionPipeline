#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include<utility>

template <typename T>
class BoundedQueue
{
private:
    // size-t is unsigned long long int
    // used for storing sizes big enough
    // cannot be negative
    // prevents code from breaking
    size_t capacity = 0;
    std::queue<T> q;
    std::mutex mtx;
    bool closed = false;
    std::condition_variable notFull;
    std::condition_variable notEmpty;

public:
    BoundedQueue() = default;

    BoundedQueue(std::size_t cap) : capacity(cap), q() {}

    bool push(const T &item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock, [&]
                     { return q.size() < capacity || closed; });
        if (closed)
            return false;
        q.push(item);
        notEmpty.notify_one();
        return true;
    }

    bool pop(T &out)
    {
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock, [&]
                      { return !q.empty() || closed; });
        if (q.empty())
            return false;
        out = std::move(q.front());
        q.pop();
        notFull.notify_one();
        return true;
    }
    void close()
    {
        std::unique_lock<std::mutex> lock(mtx);
        closed = true;
        notFull.notify_all();
        notEmpty.notify_all();
    }
};
