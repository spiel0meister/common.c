#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H
#include <stddef.h>

typedef struct {
    char* items;
    size_t size;
    size_t capacity;
}StringBuilder;

// Initializes a string builder 
StringBuilder sbuilder_init(size_t init_capacity);
// Resizes a string builder 
void sbuilder_resize(StringBuilder* sbuilder);

// Pushes a character to a string builder 
void sbuilder_push(StringBuilder* sbuilder, char c);

void sbuilder_push_str_(StringBuilder* sbuilder, ...);

// Pushes a variadic amount of cstrings to a string builder 
#define sbuilder_push_str(sb, ...) sbuilder_push_str_(sb, __VA_ARGS__, NULL)

// Pushes a sized string of certain length to a string builder 
void sbuilder_push_nstr(StringBuilder* self, const char* str, size_t len);

// Exports the contents of a string builder to heap memory 
char* sbuilder_export(StringBuilder const* sbuilder);
// Exports the contents of a string builder to to provided DST 
void sbuilder_export_inplace(StringBuilder const* sbuilder, char* dst);

// Frees a string builder
void sbuilder_free(StringBuilder* sbuilder);

#endif // STRING_BUILDER_H

#ifdef SBUILDER_IMPLEMENTATION
#ifndef SBUILDER_MALLOC
    #define SBUILDER_MALLOC(n) malloc(sizeof(char) * (n))
#endif // SBUILDER_MALLOC

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

StringBuilder sbuilder_init(size_t init_capacity) {
    StringBuilder sb = {
        malloc(init_capacity),
        0,
        init_capacity,
    };
    return sb;
}

void sbuilder_resize(StringBuilder* sbuilder) {
    sbuilder->capacity = sbuilder->capacity == 0 ? 2 : sbuilder->capacity * 2;
    sbuilder->items = realloc(sbuilder->items, sizeof(sbuilder->items[0]) * sbuilder->capacity);
    assert(sbuilder->items != NULL);
}

void sbuilder_push(StringBuilder* sbuilder, char c) {
    if (sbuilder->size + 1 >= sbuilder->capacity) {
        sbuilder_resize(sbuilder);
    }
    sbuilder->items[sbuilder->size++] = c;
}

void sbuilder_push_str_(StringBuilder* sbuilder, ...) {
    va_list list;
    va_start(list, sbuilder);

    char* cstr = va_arg(list, char*);
    while (cstr != NULL) {
        int len = strlen(cstr);
        while (sbuilder->size + len >= sbuilder->capacity) {
            sbuilder_resize(sbuilder);
        }
        memcpy(sbuilder->items + sbuilder->size, cstr, len);
        sbuilder->size += len;
        cstr = va_arg(list, char*);
    }

    va_end(list);
}

void sbuilder_push_nstr(StringBuilder* self, const char* str, size_t len) {
    while (self->size + len >= self->capacity) sbuilder_resize(self);
    memcpy(self->items + self->size, str, len);
    self->size += len;
}

char* sbuilder_export(StringBuilder const* sbuilder) {
    char* heap_mem = SBUILDER_MALLOC(sbuilder->size + 1);
    memcpy(heap_mem, sbuilder->items, sbuilder->size);
    heap_mem[sbuilder->size] = 0;
    return heap_mem;
}

void sbuilder_export_inplace(StringBuilder const* sbuilder, char* dst) {
    memcpy(dst, sbuilder->items, sbuilder->size);
}

void sbuilder_free(StringBuilder* sbuilder) {
    sbuilder->size = 0;
    sbuilder->capacity = 0;
    free(sbuilder->items);
}

#endif // STRING_BUILDER_IMPLENTATION

