#include "spmc_naive_fifo.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>
#include <array>
#include <atomic>
#include <future>

TEST(spmc_naive_fifo_test, single_thread)
{
    struct T { int x; T(int x):x(x){} };
    spmc_naive_fifo<T> queue;

    constexpr int N = 1000;
    for(size_t i = 0; i < N; ++i)
    {
        queue.push(i);
    }

    for(size_t i = 0; i < N; ++i)
    {
        EXPECT_EQ(i, queue.pop().x);
    }
}

TEST(spmc_naive_fifo_test, threaded)
{
    struct T { int x; T(int x):x(x){} };
    spmc_naive_fifo<T> queue;
    constexpr int N = 1000;

    std::atomic<int> consumed;

    // create contention point but spawning N consumers,
    // each will read 2 values from the queue
    std::array<std::thread, N> consumers;
    for(size_t i = 0; i < N; ++i)
    {
        consumers[i] = std::move(std::thread(
                [&]{
                    queue.pop();
                    consumed++;
                    queue.pop();
                    consumed++;
                }));
    }

    // now add producer
    std::thread producer{
        [&]{
            for(size_t i = 0; i < 2*N; ++i)
            {
                queue.push(i);
            }
        }};

    // wait for all threads
    producer.join();
    for(auto & c : consumers) c.join();

    // check if all was consumed
    EXPECT_EQ(2*N, consumed);
}
