#pragma once

#include<mutex>
#include<utility>
#include<queue>
#include<condition_variable>

template<typename T>
class mpmc_naive_fifo
{
public:
    template<typename... Args>
        void push_wake_all(Args&&... args)
        {
            std::unique_lock<std::mutex> lock(mutex);
            queue.emplace(std::forward<Args>(args)...);
            lock.unlock();
            cv.notify_all();
        }

    template<typename... Args>
        void push_wake_one(Args&&... args)
        {
            std::unique_lock<std::mutex> lock(mutex);
            queue.emplace(std::forward<Args>(args)...);
            lock.unlock();
            cv.notify_one();
        }

    template<typename... Args>
        void push(Args&&... args) { push_wake_one(std::forward<Args>(args)...); }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        do cv.wait(lock, [this](){ return !queue.empty(); });
        while ( queue.empty() );
        T r = queue.front();
        queue.pop();
        return r;
    }

    std::mutex mutex;
    std::queue<T> queue;
    std::condition_variable cv;
};
