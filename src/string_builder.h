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
char* sbuilder_export_raw(StringBuilder const* sbuilder);

HeapString shortf(char const* fmt, ...);
void shortf_raw(char* out, char const* fmt, ...);

#ifdef SBUILDER_IMPLEMENETATION
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

HeapString shortf(char const* fmt, ...) {
#ifndef TEXTBUF_LEN
    #define TEXTBUF_LEN 1024
#endif 
    char buf[TEXTBUF_LEN] = {0};
    size_t size = 0;

    va_list list;
    va_start(list, fmt);
    int written = vsnprintf(buf, TEXTBUF_LEN, fmt, list);
    va_end(list);
    // Stolen from https://github.com/raysan5/raylib/blob/9d67f4734b59244b5b10d45ce7c8eed76323c3b5/src/rtext.c#L1427
    if (written >= TEXTBUF_LEN) {
        char *truncBuffer = buf + TEXTBUF_LEN - 4; // Adding 4 bytes = "...\0"
        sprintf(truncBuffer, "...");
        size = TEXTBUF_LEN - 1; 
    } else if (written < 0) {
        size = 0;
    } else {
        size = written;
    }

    HeapString ret = {
        SBUILDER_MALLOC(size + 1),
        size
    };
    memcpy(ret.items, buf, size);
    ret.items[size] = 0;
    return ret;
}

char* shortf_raw(char const* fmt, ...) {
#ifndef TEXTBUF_LEN
    #define TEXTBUF_LEN 1024
#endif 
    char buf[TEXTBUF_LEN] = {0};
    size_t size = 0;

    va_list list;
    va_start(list, fmt);
    int written = vsnprintf(buf, TEXTBUF_LEN, fmt, list);
    va_end(list);
    // Stolen from https://github.com/raysan5/raylib/blob/9d67f4734b59244b5b10d45ce7c8eed76323c3b5/src/rtext.c#L1427
    if (written >= TEXTBUF_LEN) {
        char *truncBuffer = buf + TEXTBUF_LEN - 4; // Adding 4 bytes = "...\0"
        sprintf(truncBuffer, "...");
        size = TEXTBUF_LEN - 1; 
    } else if (written < 0) {
        size = 0;
    } else {
        size = written;
    }

    char* out = SBUILDER_MALLOC(size + 1);
    memcpy(ret.items, buf, size);
    ret.items[size] = 0;
    return out;
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

char* sbuilder_export_raw(StringBuilder const* sbuilder) {
    char* heap_mem = SBUILDER_MALLOC(sbuilder->size + 1);
    memcpy(heap_mem, sbuilder->items, sbuilder->size);
    heap_mem[sbuilder->size] = 0;
    return heap_mem;
}

#endif // STRING_BUILDER_IMPLENTATION

#endif // STRING_BUILDER_H
