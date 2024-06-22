#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>

typedef struct {
    char* mem;
    size_t size;
    size_t capacity;
}Arena;

#define arena_alloc(arena, Type) (Type*)arena_alloc_s(arena, sizeof(Type))
#define arena_calloc(arena, Type) (Type*)arena_calloc_s(arena, sizeof(Type))

#define arena_alloc_array(arena, n, Type) (Type*)arena_alloc_s(arena, sizeof(Type) * (n))
#define arena_calloc_array(arena, n, Type) (Type*)arena_calloc_s(arena, sizeof(Type) * (n))

void arena_prealloc(Arena* arena, size_t capacity);

void* arena_alloc_s(Arena* arena, size_t size);
void* arena_calloc_s(Arena* arena, size_t size);
void* arena_memdup(Arena* arena, void* mem, size_t size);

void arena_reset(Arena* arena);
void arena_free(Arena* arena);

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

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

void* arena_calloc_s(Arena* arena, size_t size) {
    void* p = arena_alloc_s(arena, size);
    memset(p, 0, size);
    return p;
}

void* arena_memdup(Arena* arena, void* mem, size_t size) {
    void* dup = arena_alloc_s(arena, size);
    memcpy(dup, mem, size);
    return dup;
}

char* arena_strdup(Arena* arena, const char* cstr) {
    size_t size = strlen(cstr);
    char* dup = arena_alloc_array(arena, size, char);
    memcpy(dup, cstr, size);
    return dup;
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

