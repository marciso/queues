#include "utils.hpp"

//#define _GNU_SOURCE
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>

#include<fstream>
#include<cstring>

bool this_thread_set_affinity(unsigned int cpu)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    if(int err = ::pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset)) {
        log_error("ERROR: failed to set affinity to CPU %d: %s\n", cpu, ::strerror(err));
        return false;
    }
    return true;
}

bool this_thread_set_realtime_priority(int prio, int policy)
{
    struct sched_param params;
    params.sched_priority = prio;

    if(int err = ::pthread_setschedparam(pthread_self(), policy, &params)) {
        log_error("ERROR: pthread_setschedparam(policy=%d, priority=%d): %s (are you root?)\n",
                policy, prio, ::strerror(err));
        return false;
    }
    return true;
}

bool is_tcs_clocksource_active()
{
    char const * const filename =
        "/sys/devices/system/clocksource/clocksource0/current_clocksource";
    std::ifstream current_clocksource(filename);

    std::string line;
    if (std::getline(current_clocksource, line)) {
        return line == "tsc";
    }
    return false;
}

void print_clock_res()
{
    struct timespec t;
    if(::clock_getres(CLOCK_REALTIME, &t)) {
        perror("clock_getres(CLOCK_REALTIME)");
        return;
    }
    printf("clock_gettime(CLOCK_REALTIME) resolution: %ld.%09lds\n", t.tv_sec, t.tv_nsec);
}
