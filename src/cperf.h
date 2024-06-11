#ifndef PERF_H
#define PERF_H
#include <time.h>

typedef struct timespec Timespec;

void perf_timestamp(Timespec* ts);
long perf_diff_nanoseconds(Timespec* start);
double perf_diff_seconds(Timespec* start);

#endif // PERF_H

#ifdef PERF_IMPLEMENTATION
void perf_timestamp(Timespec* ts) {
    timespec_get(ts, TIME_UTC);
}

long perf_diff_nanoseconds(Timespec* start) {
    Timespec now;
    perf_timestamp(&now);
    long delta_sec = now.tv_sec - start->tv_sec;
    long delta_nsec = now.tv_nsec - start->tv_nsec;
    return delta_sec * 1e9 + delta_nsec; 
}

double perf_diff_seconds(Timespec* start) {
    Timespec now;
    perf_timestamp(&now);
    long delta_sec = now.tv_sec - start->tv_sec;
    long delta_nsec = now.tv_nsec - start->tv_nsec;
    return delta_sec + delta_nsec * 1e-9; 
}
#endif
