#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H
#include <stddef.h>
#include <stdbool.h>

#ifndef SB_INIT_CAP
#define SB_INIT_CAP 8
#endif // SB_INIT_CAP

typedef struct {
    char* items;
    size_t count;
    size_t capacity;
}StringBuilder;

// Resizes a string builder 
void sb_maybe_resize(StringBuilder* sb, size_t to_append_len);

// Pushes a character to a string builder 
void sb_push(StringBuilder* sb, char c);

void sb_push_str_null(StringBuilder* sb, ...);
// Pushes a variadic amount of cstrings to a string builder 
#define sb_push_str(sb, ...) sb_push_str_null(sb, __VA_ARGS__, NULL)

// Pushes a sized string of certain length to a string builder 
void sb_push_nstr(StringBuilder* self, const char* str, size_t len);

// Reads the provided file and appends its contents into the provided SB.
bool sb_read_file(StringBuilder* sb, const char* filepath); 

// Exports the contents of a string builder to heap memory 
char* sb_export(StringBuilder const* sb);

// Exports the contents of a string builder to to provided DST 
void sb_export_inplace(StringBuilder const* sb, char dst[sb->count]);

// Frees a string builder
void sb_free(StringBuilder* sb);

#endif // STRING_BUILDER_H

#ifdef SB_IMPLEMENTATION
#ifndef SB_MALLOC
    #define SB_MALLOC malloc
#endif // SB_MALLOC

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void sb_maybe_resize(StringBuilder* sb, size_t to_append_len) {
    if (sb->count + to_append_len >= sb->capacity) {
        if (sb->capacity == 0) sb->capacity = SB_INIT_CAP;
        while (sb->count + to_append_len >= sb->capacity) {
            sb->capacity *= 2;
        }

        sb->items = realloc(sb->items, sizeof(sb->items[0]) * sb->capacity);
        assert(sb->items != NULL);
    }
}

void sb_push(StringBuilder* sb, char c) {
    sb_maybe_resize(sb, 1);
    sb->items[sb->count++] = c;
}

void sb_push_str_null(StringBuilder* sb, ...) {
    va_list list;
    va_start(list, sb);

    const char* cstr = va_arg(list, const char*);
    while (cstr != NULL) {
        int len = strlen(cstr);
        sb_maybe_resize(sb, len);
        memcpy(sb->items + sb->count, cstr, len);
        sb->count += len;
        cstr = va_arg(list, char*);
    }

    va_end(list);
}

void sb_push_nstr(StringBuilder* sb, const char* str, size_t len) {
    sb_maybe_resize(sb, len);
    memcpy(sb->items + sb->count, str, len);
    sb->count += len;
}

bool sb_read_file(StringBuilder* sb, const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) return false;

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    sb_maybe_resize(sb, n);
    int n_ = fread(sb->items, 1, n, f);
    assert(n_ == (int)n);
    sb->count += n_;

    fclose(f);
    return true;
} 

char* sb_export(StringBuilder const* sb) {
    char* out = SB_MALLOC(sizeof(char) * (sb->count + 1));
    memcpy(out, sb->items, sb->count);
    out[sb->count] = 0;
    return out;
}

void sb_export_inplace(StringBuilder const* sb, char dst[sb->count]) {
    memcpy(dst, sb->items, sb->count);
}

void sb_free(StringBuilder* sb) {
    free(sb->items);
    sb->items = NULL;
    sb->count = 0;
    sb->capacity = 0;
}

#endif // STRING_BUILDER_IMPLENTATION

