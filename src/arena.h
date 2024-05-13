#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>

typedef struct {
    char* mem;
    size_t size;
    size_t capacity;
}Arena;

#define arena_alloc(arena, type) (type*)arena_alloc_s(arena, sizeof(type))
#define arena_alloc_array(arena, type, n) (type*)arena_alloc_s(arena, sizeof(type) * (n))
#define arena_calloc(arena, type) (type*)arena_calloc_s(arena, sizeof(type))
#define arena_calloc_array(arena, type, n) (type*)arena_calloc_s(arena, sizeof(type) * (n))

#define arena_dump_from_ptr(arena, sink, type, ptr) arena_dump(arena, sink, (ptr) - (arena)->mem, sizeof(type))
#define arena_dump_all(arena, sink) arena_dump(arena, sink, 0, (arena)->size)

void arena_prealloc(Arena* arena, size_t capacity);

void* arena_alloc_s(Arena* arena, size_t size);
void* arena_calloc_s(Arena* arena, size_t size);

void arena_dump(Arena* arena, FILE* sink, size_t i, size_t n);
void arena_reset(Arena* arena);
void arena_free(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION
#include <stdlib.h>

void arena_prealloc(Arena* arena, size_t capacity) {
    arena->mem = malloc(capacity);
    arena->size = 0;
    arena->capacity = capacity;
}

void* arena_calloc_s(Arena* arena, size_t size) {
    void* p = arena_alloc_s(arena, size);
    memset(p, 0, size);
    return p;
}

void* arena_alloc_s(Arena* arena, size_t size) {
    if (arena->size + size > arena->capacity) {
        return NULL;
    }
    size_t i = arena->size;
    arena->size += size;
    return arena->mem + i;
}

void arena_dump(Arena* arena, FILE* sink, size_t i, size_t n) {
    char* start = (char*)&arena->mem[i];
    for (size_t j = 0; j < n; ++j) {
        if (j != n - 1) {
            fprintf(sink, "%02x ", *start);
        } else {
            fprintf(sink, "%02x\n", *start);
        }
        start++;
    }
}

void arena_reset(Arena* arena) {
    arena->size = 0;
}

void arena_free(Arena* arena) {
    free(arena->mem);
    arena->mem = NULL;
    arena->size = 0;
    arena->capacity = 0;
}
#endif // ARENA_IMPLEMENTATION
