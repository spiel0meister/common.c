#include <stdlib.h>
#include <time.h>

typedef struct Perf Perf;

Perf* perf_start(const char* label);

double perf_timestamp_secs(Perf* perf);
long perf_timestamp_nsecs(Perf* perf);

void perf_log_secs(Perf* perf);
void perf_log_nsecs(Perf* perf);

double perf_end_secs(Perf* perf);
long perf_end_nsecs(Perf* perf);

#ifdef PERF_IMPLEMENTATION
struct Perf {
    const char* label;
    struct timespec start;
};

#ifndef PERF_MALLOC
    #define PERF_MALLOC malloc
#endif

#ifndef PERF_FREE
    #define PERF_FREE free
#endif
Perf* perf_start(const char* label) {
    Perf* perf = PERF_MALLOC(sizeof(*perf));

    perf->label = label;
    clock_gettime(CLOCK_MONOTONIC, &perf->start);

    return perf;
}

static struct timespec ts_now() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now;
}

double perf_timestamp_secs(Perf* perf) {
    struct timespec now = ts_now();

    double start_secs = perf->start.tv_sec + perf->start.tv_nsec * 1e-9;
    double now_secs = now.tv_sec + now.tv_nsec * 1e-9;
    return now_secs - start_secs;
}

long perf_timestamp_nsecs(Perf* perf) {
    struct timespec now = ts_now();

    long start_nsecs = perf->start.tv_sec * 1e9 + perf->start.tv_nsec;
    long now_nsecs = now.tv_sec * 1e9 + now.tv_nsec;
    return now_nsecs - start_nsecs;
}

void perf_log_secs(Perf* perf) {
    printf("%s: %fs\n", perf->label, perf_timestamp_secs(perf));
}

void perf_log_nsecs(Perf* perf) {
    printf("%s: %ldns\n", perf->label, perf_timestamp_nsecs(perf));
}

double perf_end_secs(Perf* perf) {
    double secs = perf_timestamp_secs(perf);
    PERF_FREE(perf);
    return secs;
}

long perf_end_nsecs(Perf* perf) {
    long nsecs = perf_timestamp_nsecs(perf);
    PERF_FREE(perf);
    return nsecs;
}
#endif // PERF_IMPLEMENTATION
