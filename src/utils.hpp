#pragma once
#include<cstdlib>
#include<cstdio>
#include<thread>

#define unlikely(x)     __builtin_expect((x),0)
#define likely(x)       __builtin_expect((x),1)

#define HOT __attribute__((hot))
#define COLD __attribute__((cold))

#define INLINE __attribute__((always_inline))


#define log_info(...) do { fprintf(stdout, __VA_ARGS__); } while(false)
#define log_error(...) do { fprintf(stderr, __VA_ARGS__); } while(false)
#define fatal_error(...) do { fprintf(stderr, __VA_ARGS__); std::abort(); } while(false)

// return true for 0 as well
constexpr inline bool is_power_of_2(size_t n) { return ((n-1) & n) == 0; }

bool this_thread_set_affinity(unsigned int cpu);
bool is_tcs_clocksource_active();
bool this_thread_set_realtime_priority(int prio, int policy /* e.g. SCHED_RR */);
void print_clock_res();

inline void INLINE compiler_barrier() { asm volatile("" ::: "memory"); }

// high power pause - should work on all i386
inline void INLINE cpu_relax() { asm volatile("rep; nop" ::: "memory"); }

// CPU enters lower power level, supported on P4 and higher
inline void INLINE cpu_pause() { asm volatile("pause" ::: "memory"); }

inline void INLINE this_thread_yield()
{
    std::this_thread::yield(); // an alternative is sched_yield() from <sched.h>
    // or pthread_yield() from <pthread.h>
}

inline uint64_t INLINE rdtsc()
{
      uint32_t hi, lo;
      asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
      return ((uint64_t)lo)|(((uint64_t)hi)<<32);
}
