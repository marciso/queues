#include "spsc_bounded_fifo.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>
#include <array>
#include <atomic>
#include <future>

TEST(spsc_bounded_fifo_test, single_thread)
{
    struct T { int x; T(int x = 0):x(x){} };
    constexpr size_t N = 1000;
    spsc_bounded_fifo<T, 1024> queue;

    for(size_t i = 0; i < N; ++i)
    {
        queue.push(i);
    }

    for(size_t i = 0; i < N; ++i)
    {
        T t;
        EXPECT_TRUE(queue.pop(t));
        EXPECT_EQ(i, t.x);
    }
}

TEST(spsc_bounded_fifo_test, threaded_1c1p)
{
    struct T { int x; T(int x = 0):x(x){} };
    constexpr size_t N = 1000;
    spsc_bounded_fifo<T, 1024> queue;

    // create contention point but spawning N consumers,
    // each will read 2 values from the queue
    std::thread consumer{
        [&]{
            for(size_t i = 0; i < N; ++i)
            {
                T t;
                while( !queue.pop(t) );
                EXPECT_EQ(i, t.x);
            }
        }};

    // now add producer
    std::thread producer{
        [&]{
            for(size_t i = 0; i < N; ++i)
            {
                queue.push(i);
            }
        }};

    // wait for all threads
    producer.join();
    consumer.join();
}
