#ifndef ARENA_H_
#define ARENA_H_
#include <stdint.h>
#include <stdlib.h>

#ifndef REGION_DEFAULT_SIZE
#define REGION_DEFAULT_SIZE (1 << 10)
#endif // REGION_DEFAULT_SIZE

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC malloc
#endif // ARENA_MALLOC 

#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE free
#endif // ARENA_FREE 

typedef struct arena_region_s {
    size_t capacity;
    size_t count;
    struct arena_region_s* next;

    uintptr_t data[];
}ArenaRegion;

typedef struct {
    ArenaRegion *start, *end;
}Arena;

typedef struct {
    ArenaRegion* r;
    size_t count;
}ArenaMark;

void* arena_alloc(Arena* self, size_t size);
#define arena_calloc(arena, size) memset(arena_alloc(arena, size), 0, size)
void* arena_realloc(Arena* self, size_t oldsize, size_t newsize, void* ptr);
#define arena_memdup(arena, ptr, size) memcpy(arena_alloc(arena, size), ptr, size)
char* arena_sprintf(Arena* self, const char* fmt, ...);
char* arena_strdup(Arena* self, const char* cstr);

ArenaMark arena_mark(Arena* self);
void arena_jumpback(Arena* self, ArenaMark mark);
void arena_reset(Arena* self);
void arena_free(Arena* self);

#endif // ARENA_H_

#ifdef ARENA_IMPLEMENTATION
#undef ARENA_IMPLEMENTATION

#include <stdarg.h>
#include <string.h>
#include <assert.h>

void* arena_alloc(Arena* self, size_t size) {
    if (self->end == NULL) {
        self->end = ARENA_MALLOC(sizeof(*self->start) + REGION_DEFAULT_SIZE * sizeof(*self->start->data));
        self->end->count = 0;
        self->end->capacity = REGION_DEFAULT_SIZE;
        self->end->next = NULL;
        self->start = self->end;
    }
    assert(self->end != NULL);

    while (self->end->count + size > self->end->capacity && self->end->next != NULL) {
        self->end = self->end->next;
    }

    if (self->end->count + size > self->end->capacity) {
        self->end->next = ARENA_MALLOC(sizeof(*self->end->next) + REGION_DEFAULT_SIZE * sizeof(*self->start->data));
        self->end->next->count = 0;
        self->end->next->capacity = REGION_DEFAULT_SIZE;
        self->end->next->next = NULL;
        self->end = self->end->next;
    }

    size_t word_size = sizeof(uintptr_t);
    size_t realsize = (size + word_size - 1) / word_size;

    void* mem = self->end->data + self->end->count;
    self->end->count += realsize;
    return mem;
}

void* arena_realloc(Arena* self, size_t oldsize, size_t newsize, void* ptr) {
    void* newptr = arena_alloc(self, newsize);
    return memcpy(newptr, ptr, oldsize);
}

char* arena_sprintf(Arena* self, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

#pragma GCC diagnostic ignored "-Wformat-truncation"
    int n = vsnprintf(NULL, 0, fmt, args);

    va_end(args);
    
    char* cstr = arena_alloc(self, n);

    va_start(args, fmt);

    int n_ = vsnprintf(NULL, n + 1, fmt, args);
    assert(n_ == n);

    va_end(args);

    return cstr;
}

char* arena_strdup(Arena* self, const char* cstr) {
    size_t len = strlen(cstr) + 1;
    char* copy = arena_alloc(self, len);
    return memcpy(copy, cstr, len);
}

void arena_reset(Arena* self) {
    for (ArenaRegion* r = self->start; r != NULL; r = r->next) {
        r->count = 0;
    }

    self->end = self->start;
}

ArenaMark arena_mark(Arena* self) {
    ArenaMark mark = {self->end, self->end->count};
    return mark;
}

void arena_jumpback(Arena* self, ArenaMark mark) {
    if (mark.r == NULL) {
        arena_reset(self);
        return;
    }

    mark.r->count = mark.count;
    for (ArenaRegion* r = mark.r; r != NULL; r = r->next) {
        r->count = 0;
    }

    self->end = mark.r;
}

void arena_free(Arena* self) {
    ArenaRegion* r = self->start;
    while (r != NULL) {
        ArenaRegion* n = r->next;
        ARENA_FREE(r);
        r = n;
    }
    self->start = NULL;
    self->end = NULL;
}
#endif // ARENA_IMPLEMENTATION
