#ifndef LEAKCHECK_H_
#define LEAKCHECK_H_
#include <stdio.h>

#ifndef LC_ALLOCS_CAP
#define LC_ALLOCS_CAP 1024
#endif // LC_ALLOCS_CAP

void lcfree(void* ptr);
void* lcmalloc_(size_t size, const char* file, size_t line);
void* lccalloc_(size_t size, size_t count, const char* file, size_t line);
void* lcrealloc_(void* ptr, size_t new_size, const char* file, size_t line);
void lcprint_leaks(FILE* sink);

#endif // LEAKCHECK_H_

#ifdef LC_IMPLEMENTATION
#undef LC_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
    const char* file;
    size_t line;
    size_t size;
    char data[];
}LeakcheckInfo;

static LeakcheckInfo* lc_infos[LC_ALLOCS_CAP] = {0};
static size_t lc_infos_count = 0;

void lcprint_leaks(FILE* sink) {
    for (size_t i = 0; i < lc_infos_count; ++i) {
        LeakcheckInfo* info = lc_infos[i];
        fprintf(sink, "Leaked memory at %s:%ld of size %ld (%ld with metadata)\n", info->file, info->line, info->size, info->size + sizeof(*info));
    }
}

void lc_append(LeakcheckInfo* info) {
    assert(lc_infos_count < LC_ALLOCS_CAP);
    lc_infos[lc_infos_count++] = info;
}

void lc_remove(LeakcheckInfo* info) {
    assert(lc_infos_count > 0);
    for (size_t i = 0; i < lc_infos_count; ++i) {
        if (lc_infos[i] == info) {
            lc_infos[i] = lc_infos[lc_infos_count - 1];
            lc_infos_count -= 1;
            break;
        }
    }
}

void* lcmalloc_(size_t size, const char* file, size_t line) {
    LeakcheckInfo* info = malloc(sizeof(LeakcheckInfo) + size);
    info->file = file;
    info->line = line;
    info->size = size;
    lc_append(info);
    return info->data;
}

void* lccalloc_(size_t count, size_t size, const char* file, size_t line) {
    LeakcheckInfo* info = malloc(sizeof(LeakcheckInfo) + count * size);
    info->file = file;
    info->line = line;
    info->size = count * size;
    lc_append(info);
    return memset(info->data, 0, info->size);
}

void* lcrealloc_(void* ptr, size_t new_size, const char* file, size_t line) {
    lc_remove((LeakcheckInfo*)ptr - 1);
    LeakcheckInfo* info = realloc((LeakcheckInfo*)ptr - 1, new_size + sizeof(LeakcheckInfo));
    info->line = line;
    info->file = file;
    info->size = new_size;
    lc_append(info);
    return info->data;
}

void lcfree(void* ptr) {
    if (ptr != NULL) {
        LeakcheckInfo* info = ((LeakcheckInfo*)ptr - 1);
        lc_remove(info);
        free(info);
    }
}

#endif // LEAKCHECK_IMPLEMENTATION

#ifndef LC_NO_SHADOW_STDLIB_FUNCS
#define malloc(size) lcmalloc_(size, __FILE__, __LINE__)
#define calloc(count, size) lccalloc_(count, size, __FILE__, __LINE__)
#define realloc(ptr, new_size) lcrealloc_(ptr, new_size, __FILE__, __LINE__)
#define free lcfree
#else
#define lcmalloc(...) lcmalloc_(__VA_ARGS__, __FILE__, __LINE__)
#define lccalloc(count, size) lccalloc_(count, size, __FILE__, __LINE__)
#define lcrealloc(ptr, new_size) lcrealloc_(ptr, new_size, __FILE__, __LINE__)
#endif // LC_ALIAS_FUNCS

