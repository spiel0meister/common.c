#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H
#include <stddef.h>

typedef struct {
    char* items;
    int size;
    int capacity;
}StringBuilder;

StringBuilder sbuilder_init(size_t init_capacity);
void sbuilder_resize(StringBuilder* sbuilder);

void sbuilder_push(StringBuilder* sbuilder, char c);
void sbuilder_push_str_(StringBuilder* sbuilder, ...);
#define sbuilder_push_str(sb, ...) sbuilder_push_str_(sb, __VA_ARGS__, NULL)

char* sbuilder_export(StringBuilder const* sbuilder);
void sbuilder_export_inplace(StringBuilder const* sbuilder, char* dst);

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

