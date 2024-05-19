#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H
#include <stddef.h>

typedef struct {
    char* items;
    int size;
    int capacity;
}StringBuilder;

typedef struct {
    char* items;
    int size;
}HeapString;

StringBuilder sbuilder_init(size_t init_capacity);
void sbuilder_resize(StringBuilder* sbuilder);

void sbuilder_push(StringBuilder* sbuilder, char c);
void sbuilder_push_str_(StringBuilder* sbuilder, ...);
#define sbuilder_push_str(sb, ...) sbuilder_push_str_(sb, __VA_ARGS__, NULL)

#ifndef SBUILDER_MALLOC
    #define SBUILDER_MALLOC(n) malloc(sizeof(char) * (n))
#endif // SBUILDER_MALLOC

HeapString sbuilder_export(StringBuilder const* sbuilder);

#ifdef SBUILDER_IMPLEMENETATION
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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
        size_t len = strlen(cstr);
        while (sbuilder->size + len >= sbuilder->capacity) {
            sbuilder_resize(sbuilder);
        }
        memcpy(sbuilder->items + sbuilder->size, cstr, len);
        sbuilder->size += len;
        cstr = va_arg(list, char*);
    }

    va_end(list);
}

HeapString sbuilder_export(StringBuilder const* sbuilder) {
    char* heap_mem = SBUILDER_MALLOC(sbuilder->size + 1);
    memcpy(heap_mem, sbuilder->items, sbuilder->size);
    heap_mem[sbuilder->size] = 0;
    return (HeapString) {
        heap_mem,
        sbuilder->size
    };
}

#endif // STRING_BUILDER_IMPLENTATION

#endif // STRING_BUILDER_H
