#ifndef ARENA_H_
#define ARENA_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT(expr) assert(expr)
#endif // ARENA_ASSERT

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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
#define arena_can_alloc(arena, Type) arena_can_allocb(arena, sizeof(Type))

Arena arena_new(size_t capacity);
void arena_prealloc(Arena* self, size_t capacity);
char* arena_strdup(Arena* self, const char* cstr);
char* arena_sprintf(Arena* self, const char* restrict fmt, ...);

void arena_free(Arena* self);
void arena_reset(Arena* self);
#define arena_reset(arena) (arena)->size = 0

void* arena_allocb(Arena* self, size_t size);
void* arena_callocb(Arena* self, size_t size);
void* arena_memdupb(Arena* self, void* mem, size_t size);
bool arena_can_allocb(Arena* self, size_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ARENA_H_

#ifdef ARENA_IMPLEMENTATION
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

Arena arena_new(size_t capacity) {
    Arena a = {0};
    arena_prealloc(&a, capacity);
    return a;
}

void arena_prealloc(Arena* self, size_t capacity) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_capacity = (capacity + word_size - 1)/word_size;

    self->mem = malloc(real_capacity * word_size);
    self->size = 0;
    self->capacity = real_capacity;
}

void* arena_allocb(Arena* self, size_t size) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;

    if (self->size + real_size > self->capacity) {
        return NULL;
    }

    size_t i = self->size;
    self->size += real_size;
    return self->mem + i;
}

void* arena_callocb(Arena* self, size_t size) {
    void* p = arena_allocb(self, size);
    memset(p, 0, size);
    return p;
}

void* arena_memdupb(Arena* self, void* mem, size_t size) {
    void* dup = arena_allocb(self, size);
    if (dup == NULL) return NULL;

    memcpy(dup, mem, size);
    return dup;
}

char* arena_strdup(Arena* self, const char* cstr) {
    size_t size = strlen(cstr);
    char* dup = arena_alloc_array(self, size, char);
    if (dup == NULL) return NULL;

    memcpy(dup, cstr, size);
    return dup;
}

char* arena_sprintf(Arena* restrict self, const char* restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    va_start(args, fmt);
    char* cstr = arena_alloc_array(self, n + 1, char);
    vsnprintf(cstr, n + 1, fmt, args);
    va_end(args);

    return cstr;
}

bool arena_can_allocb(Arena* a, size_t size) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;
    return a->size + real_size < a->capacity;
}

void arena_free(Arena* self) {
    free(self->mem);
    self->mem = NULL;
    self->size = 0;
    self->capacity = 0;
}
#endif // ARENA_IMPLEMENTATION

