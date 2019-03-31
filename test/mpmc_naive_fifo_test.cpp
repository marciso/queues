#include "mpmc_naive_fifo.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>
#include <array>
#include <atomic>
#include <future>

TEST(mpmc_naive_fifo_test, single_thread)
{
    struct T { int x; T(int x):x(x){} };
    mpmc_naive_fifo<T> queue;

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

TEST(mpmc_naive_fifo_test, threaded_1000c1p_notify_one)
{
    struct T { int x; T(int x):x(x){} };
    mpmc_naive_fifo<T> queue;
    constexpr int N = 1000;

    std::atomic<int> consumed;
    consumed = 0;

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

TEST(mpmc_naive_fifo_test, threaded_1000c1p_notify_all)
{
    struct T { int x; T(int x):x(x){} };
    mpmc_naive_fifo<T> queue;
    constexpr int N = 1000;

    std::atomic<int> consumed;
    consumed = 0;

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
                queue.push_wake_all(i);
            }
        }};

    // wait for all threads
    producer.join();
    for(auto & c : consumers) c.join();

    // check if all was consumed
    EXPECT_EQ(2*N, consumed);
}

using param = std::tuple<int, int, int>;
class mpmc_naive_fifo_test_p : public ::testing::TestWithParam<param> {};

TEST_P(mpmc_naive_fifo_test_p, threaded)
{
    struct T { int x; T(int x = 0):x(x){} };
    mpmc_naive_fifo<T> queue;

    std::atomic<int> consumed;
    consumed = 0;

    auto const [number_of_producers, number_of_consumers, N] = GetParam();

    // create contention point but spawning N consumers,
    // each will read 2 values from the queue
    std::vector<std::thread> consumers;
    std::vector<std::vector<int>> values;
    values.resize(number_of_consumers);
    for(size_t i = 0; i < number_of_consumers; ++i)
    {
        consumers.emplace_back(std::move(std::thread(
                [&, i]{
                    //queue.register_this_thread();
                    for(size_t j = 0; j < N*number_of_producers; ++j) {
                        T t = queue.pop();
                        consumed++;
                        values.at(i).push_back(t.x);
                    }
                })));
    }

    // now add producer
    std::vector<std::thread> producers;
    // now add producer
    for(size_t i = 0; i < number_of_producers; ++i)
    {
        producers.emplace_back(std::move(std::thread(
            [&, i]{
                //queue.register_this_thread();
                for(size_t j = 0; j < N*number_of_consumers; ++j)
                {
                    queue.push((i+1)*N*number_of_consumers + j+1);
                }
            })));
    }

    // wait for all threads
    for(auto & p : producers) p.join();
    for(auto & c : consumers) c.join();

    // check if all was consumed
    EXPECT_EQ(number_of_consumers*number_of_producers*N, consumed);

    for(auto const & v : values)
        EXPECT_EQ(number_of_producers*N, v.size());

#if 0
    for(auto const & vect : values)
    {
        char const * sep = ": ";
        for(auto const  v : vect) {
            std::cout << sep << v;
            sep = ", ";
        }
        std::cout << "\n";
    }
#endif

    std::set<int> all;
    for(auto const & v : values)
        all.insert(v.begin(), v.end());

    EXPECT_EQ(number_of_consumers*number_of_producers*N, all.size());
}

INSTANTIATE_TEST_CASE_P(producer_consumer, mpmc_naive_fifo_test_p,
        ::testing::Values(
            param(1,1, 1000),
            param(16,1, 1000),
            param(16,16, 1000),
            param(100,10,100),
            param(50,50,10),
            param(1,1000,1000) // 1 porducer, 1K consumers
            ));
