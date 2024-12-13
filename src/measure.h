#ifndef MEASURE_H_
#define MEASURE_H_
#include <stdio.h>

double get_now();

void measures_free();

void measure_begin(const char* name);
void measure_end_with_loc(const char* name, const char* file, int line);
#define measure_end(name) measure_end_with_loc(name, __FILE__, __LINE__)
void measures_dump(FILE* sink);
#define mesaures_print() measures_dump(stdout)

#define MEASURE(name) measure_begin(name)
#define MEASURE_END(name) measure_end(name)

typedef struct {
    double* items;
    size_t count;
    size_t capacity;
}Measurements;

typedef struct {
    double start;
    const char* name;
    Measurements ms;
}Measure;


#endif // MEASURE_H_
#ifdef MEASURE_IMPLEMENTATION
#undef MEASURE_IMPLEMENTATION

double get_now() {
    struct timespec ts;
    assert(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    return ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

#define UNIQUE_MEASURES_SIZE 1024
Measure measures[UNIQUE_MEASURES_SIZE] = {0};
size_t measures_count = 0;

void measurements_push(Measurements* ms, double t) {
    if (ms->count == ms->capacity) {
        if (ms->capacity == 0) ms->capacity = 4;
        ms->capacity *= 2;
        ms->items = realloc(ms->items, ms->capacity * sizeof(double));
    }
    ms->items[ms->count++] = t;
}

void measurements_free(Measurements* ms) {
    free(ms->items);
    ms->items = NULL;
    ms->count = 0;
    ms->capacity = 0;
}

size_t measures_push(const char* name) {
    assert(measures_count < UNIQUE_MEASURES_SIZE);
    size_t i = measures_count++;
    measures[i].name = name;
    return i;
}

bool measures_find(const char* name, size_t* out) {
    for (size_t i = 0; i < measures_count; ++i) {
        if (strcmp(measures[i].name, name) == 0) {
            *out = i;
            return true;
        }
    }
    return false;
}

void measures_free() {
    for (size_t i = 0; i < measures_count; ++i) {
        measurements_free(&measures[i].ms);
    }
    memset(measures, 0, measures_count * sizeof(measures[0]));
    measures_count = 0;
}

double measuresments_average(Measurements* ms) {
    assert(ms->count > 0);
    double avg = 0;
    for (size_t i = 0; i < ms->count; ++i) {
        avg += ms->items[i];
    }
    return avg / ms->count;
}

void measures_dump(FILE* sink) {
    for (size_t i = 0; i < measures_count; ++i) {
        Measure* m = &measures[i];
        printf("%s: Average: %lfs\n", m->name, measuresments_average(&m->ms));
        for (size_t i = 0; i < m->ms.count; ++i) {
            fprintf(sink, "    %ld. mesaurement: %lfs\n", i + 1, m->ms.items[i]);
        }
    }
}

void measure_begin(const char* name) {
    size_t i;
    if (!measures_find(name, &i)) {
        i = measures_push(name);
    }
    measures[i].start = get_now();
}

void measure_end_with_loc(const char* name, const char* file, int line) {
    size_t i;
    if (!measures_find(name, &i)) {
        fprintf(stderr, "%s:%d: Couldn't find measurement %s\n", file, line, name);
        return;
    }
    double t = get_now() - measures[i].start;
    measurements_push(&measures[i].ms, t);
}

#endif // MEASURE_IMPLEMENTATION
