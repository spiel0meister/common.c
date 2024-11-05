#ifndef LINEAR_H_
#define LINEAR_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef LINEAR_ASSERT
#include <assert.h>
#define LINEAR_ASSERT(expr) assert(expr)
#endif // LINEAR_ASSERT

#ifndef cast
#ifdef __cplusplus
#define cast(Type, thing) reinterpret_cast<Type>(thing)
#else // __cplusplus
#define cast(Type, thing) (Type)(thing)
#endif // __cplusplus
#endif // cast

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    size_t size;
    size_t capacity;
    uintptr_t* mem;
}Linear;

#define linear_alloc(linear, Type) (Type*)linear_allocb(linear, sizeof(Type))
#define linear_calloc(linear, Type) (Type*)linear_callocb(linear, sizeof(Type))

#define linear_alloc_array(linear, n, Type) (Type*)linear_allocb(linear, sizeof(Type) * (n))
#define linear_calloc_array(linear, n, Type) (Type*)linear_callocb(linear, sizeof(Type) * (n))

#define linear_occupied(linear) (LINEAR_ASSERT((linear) != NULL), (linear)->size*sizeof(uintptr_t))
#define linear_can_alloc(linear, Type) linear_can_allocb(linear, sizeof(Type))

Linear linear_new(size_t capacity);
void linear_prealloc(Linear* self, size_t capacity);
char* linear_strdup(Linear* self, const char* cstr);
char* linear_sprintf(Linear* self, const char* fmt, ...);

size_t linear_snapshot(Linear* self);
void linear_rewind(Linear* self, size_t offset);

void linear_free(Linear* self);
#define linear_reset(linear) ((linear)->size = 0)

void* linear_allocb(Linear* self, size_t size);
void* linear_callocb(Linear* self, size_t size);
void* linear_memdupb(Linear* self, void* mem, size_t size);
bool linear_can_allocb(Linear* self, size_t size);

extern Linear temp_linear;
#define temp_prealloc(size) linear_prealloc(&temp_linear, size)
#define temp_alloc(Type) linear_alloc(&temp_linear, Type)
#define temp_allocb(size) linear_allocb(&temp_linear, size)
#define temp_sprintf(...) linear_sprintf(&temp_linear, __VA_ARGS__)
#define temp_snapshot() linear_snapshot(&temp_linear)
#define temp_rewind(offset) linear_rewind(&temp_linear, offset)
#define temp_free() linear_free(&temp_linear)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LINEAR_H_

#ifdef LINEAR_IMPLEMENTATION
#undef LINEAR_IMPLEMENTATION

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

Linear temp_linear = {0};

Linear linear_new(size_t capacity) {
    Linear a = {0};
    linear_prealloc(&a, capacity);
    return a;
}

void linear_prealloc(Linear* self, size_t capacity) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_capacity = (capacity + word_size - 1)/word_size;

    self->mem = cast(uintptr_t*, malloc(real_capacity * word_size));
    self->size = 0;
    self->capacity = real_capacity;
}

size_t linear_snapshot(Linear* self) {
    return self->size;
}

void linear_rewind(Linear* self, size_t offset) {
    self->size = offset;
}

void* linear_allocb(Linear* self, size_t size) {
    assert(self->mem != NULL && "Linear in not prealloced");

    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;

    if (self->size + real_size > self->capacity) {
        return NULL;
    }

    size_t i = self->size;
    self->size += real_size;
    return cast(void*, self->mem + i);
}

void* linear_callocb(Linear* self, size_t size) {
    void* p = linear_allocb(self, size);
    memset(p, 0, size);
    return p;
}

void* linear_memdupb(Linear* self, void* mem, size_t size) {
    void* dup = linear_allocb(self, size);
    if (dup == NULL) return NULL;

    memcpy(dup, mem, size);
    return dup;
}

char* linear_strdup(Linear* self, const char* cstr) {
    size_t size = strlen(cstr);
    char* dup = linear_alloc_array(self, size, char);
    if (dup == NULL) return NULL;

    memcpy(dup, cstr, size);
    return dup;
}

char* linear_sprintf(Linear* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    va_start(args, fmt);
    char* cstr = linear_alloc_array(self, n + 1, char);
    vsnprintf(cstr, n + 1, fmt, args);
    va_end(args);

    return cstr;
}

size_t linear_get_offset(Linear* self, void* thing) {
    assert((uint8_t*)self->mem <= (uint8_t*)thing && (uint8_t*)thing < (uint8_t*)self->mem + self->capacity);

    return cast(char*, thing) - cast(char*, self->mem);
}

void* linear_exportb(Linear* self, size_t offset, size_t size) {
    void* mem = malloc(size);
    memcpy(mem, self->mem + offset, size);
    return mem;
}


bool linear_can_allocb(Linear* a, size_t size) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;
    return a->size + real_size < a->capacity;
}

void linear_free(Linear* self) {
    free(self->mem);
    self->mem = NULL;
    self->size = 0;
    self->capacity = 0;
}
#endif // LINEAR_IMPLEMENTATION

