#ifndef PERF_H_
#define PERF_H_
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

double clock_secs(clock_t start);
double clock_msecs(clock_t start);
long clock_nsecs(clock_t start);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PERF_H_

#ifdef CPERF_IMPLEMENTATION

double clock_secs(clock_t start) {
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

double clock_msecs(clock_t start) {
    return clock_secs(start) * 1000;
}

long clock_nsecs(clock_t start) {
    return clock_secs(start) * 1000000;
}

#endif
