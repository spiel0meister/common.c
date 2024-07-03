#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>
#include <stdint.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT(expr) assert(expr)
#endif // ARENA_ASSERT

typedef struct {
    size_t size;
    size_t capacity;
    uintptr_t* mem;
}Arena;

#define arena_alloc(arena, Type) (Type*)arena_allocb(arena, sizeof(Type))
#define arena_calloc(arena, Type) (Type*)arena_callocb(arena, sizeof(Type))

#define arena_alloc_array(arena, n, Type) (Type*)arena_allocb(arena, sizeof(Type) * (n))
#define arena_calloc_array(arena, n, Type) (Type*)arena_callocb(arena, sizeof(Type) * (n))

#define arena_occupied(arena) (ARENA_ASSERT((arena) != NULL), (arena)->size*sizeof(uintptr_t))

void arena_prealloc(Arena* arena, size_t capacity);

void* arena_allocb(Arena* arena, size_t size);
void* arena_callocb(Arena* arena, size_t size);
void* arena_memdupb(Arena* arena, void* mem, size_t size);
char* arena_strdup(Arena* arena, const char* cstr);
char* arena_sprintf(Arena* arena, const char* restrict fmt, ...);

void arena_reset(Arena* arena);
#define arena_reset(arena) (arena)->size = 0

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void arena_prealloc(Arena* arena, size_t capacity) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_capacity = (capacity + word_size - 1)/word_size;

    arena->mem = malloc(real_capacity * word_size);
    arena->size = 0;
    arena->capacity = real_capacity;
}

void* arena_allocb(Arena* arena, size_t size) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;

    if (arena->size + real_size > arena->capacity) {
        return NULL;
    }

    size_t i = arena->size;
    arena->size += real_size;
    return arena->mem + i;
}

void* arena_callocb(Arena* arena, size_t size) {
    void* p = arena_allocb(arena, size);
    memset(p, 0, size);
    return p;
}

void* arena_memdupb(Arena* arena, void* mem, size_t size) {
    void* dup = arena_allocb(arena, size);
    if (dup == NULL) return NULL;

    memcpy(dup, mem, size);
    return dup;
}

char* arena_strdup(Arena* arena, const char* cstr) {
    size_t size = strlen(cstr);
    char* dup = arena_alloc_array(arena, size, char);
    if (dup == NULL) return NULL;

    memcpy(dup, cstr, size);
    return dup;
}

char* arena_sprintf(Arena* restrict arena, const char* restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    va_start(args, fmt);
    char* cstr = arena_alloc_array(arena, n + 1, char);
    vsnprintf(cstr, n + 1, fmt, args);
    va_end(args);

    return cstr;
}

void arena_free(Arena* arena) {
    free(arena->mem);
    arena->mem = NULL;
    arena->size = 0;
    arena->capacity = 0;
}
#endif // ARENA_IMPLEMENTATION

