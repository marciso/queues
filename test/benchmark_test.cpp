#include <benchmark/benchmark.h>

#include "mpmc_bounded_fifo.hpp"
#include "mpmc_naive_fifo.hpp"

#include <thread>

static void BM_mpmc_bounded_fifo_push_pop(benchmark::State& state) {
    struct T { int x; T(int x = 0):x(x){} };
    mpmc_bounded_fifo<T, 1024> queue;
    queue.register_this_thread();
    T t;
    for (auto _ : state)
    {
        queue.push(1);

        queue.pop(t);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_mpmc_bounded_fifo_push_pop);

static void BM_mpmc_bounded_fifo_contended_pop(benchmark::State& state) {
    struct T { int x; T(int x = 0):x(x){} };
    mpmc_bounded_fifo<T, 1024> queue;

    std::thread producer{
        [&]{
            int i = 0;
            queue.register_this_thread();
            while(state.KeepRunning())
            {
                queue.push(i++);
            }
        }};

    queue.register_this_thread();
    T t;
    for (auto _ : state)
    {
        queue.pop(t);
    }

    producer.join();
}
BENCHMARK(BM_mpmc_bounded_fifo_contended_pop);

static void BM_mpmc_naive_fifo_push_pop(benchmark::State& state) {
    struct T { int x; T(int x = 0):x(x){} };
    mpmc_naive_fifo<T> queue;
    T t;
    for (auto _ : state)
    {
        queue.push(1);
        t = queue.pop();
    }
}
// Register the function as a benchmark
BENCHMARK(BM_mpmc_naive_fifo_push_pop);

static void BM_mpmc_naive_fifo_contended_pop(benchmark::State& state) {
    struct T { int x; T(int x = 0):x(x){} };
    mpmc_naive_fifo<T> queue;

    std::thread producer{
        [&]{
            int i = 0;
            while(state.KeepRunning())
            {
                queue.push(i++);
            }
        }};

    T t;
    for (auto _ : state)
    {
        t = queue.pop();
    }

    producer.join();
}
BENCHMARK(BM_mpmc_naive_fifo_contended_pop);
BENCHMARK_MAIN();
