#pragma once
#include<cstdlib>
#include<cstdio>

#define unlikely(x)     __builtin_expect((x),0)
#define likely(x)       __builtin_expect((x),1)

#define HOT __attribute__((hot))
#define COLD __attribute__((cold))


#define log_error(...) do { fprintf(stderr, __VA_ARGS__); } while(false)
#define fatal_error(...) do { fprintf(stderr, __VA_ARGS__); std::abort(); } while(false)

// return true for 0 as well
constexpr inline bool is_power_of_2(size_t n) { return ((n-1) & n) == 0; }

bool this_thread_set_affinity(unsigned int cpu);
bool is_tcs_clocksource_active();
bool this_thread_set_realtime_priority(int prio, int policy /* e.g. SCHED_RR */);
void print_clock_res();
