#ifndef TSPRINTF_H_
#define TSPRINTF_H_
#include <stddef.h>

char* tsprintf(const char* fmt, ...);
size_t tsnapshot();
void trewind(size_t snapshot);
void treset();

#endif // TSPRINTF_H_
#ifdef TSPRINTF_IMPLEMENTATION
#include <assert.h>
#include <stdarg.h>

#ifndef TBUFFER_SIZE
#define TBUFFER_SIZE (1 << 10)
#endif // TBUFFER_SIZE

static char tbuffer[TBUFFER_SIZE] = {0};
static size_t tbuffer_size = 0;

char* tsprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (n + tbuffer_size >= TBUFFER_SIZE) {
        return NULL;
    }

    char* start = tbuffer + tbuffer_size;

    va_start(args, fmt);
    int n_ = vsnprintf(start, n + 1, fmt, args);
    assert(n == n_);

    tbuffer_size += n_;
    va_end(args);

    return start;
}

size_t tsnapshot() {
    return tbuffer_size;
}

void trewind(size_t snapshot) {
    tbuffer_size = snapshot;
}

void treset() {
    tbuffer_size = 0;
}

#endif // TSPRINTF_IMPLEMENTATION
