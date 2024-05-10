#ifndef ARENA_H
#define ARENA_H
#include <stddef.h>

typedef struct {
    void* mem;
    size_t size;
    size_t capacity;
}Arena;

#define arena_alloc(arena, type) arena_alloc_s(arena, sizeof(type))

Arena arena_init(size_t capacity);
void* arena_alloc_s(Arena* arena, size_t size);
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

void* arena_alloc_s(Arena* arena, size_t size) {
    if (arena->size + size > arena->capacity) {
        return NULL;
    }
    size_t i = arena->size;
    arena->size += size;
    return arena->mem + i;
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
