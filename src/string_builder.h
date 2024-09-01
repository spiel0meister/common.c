#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef SB_INIT_CAP
#define SB_INIT_CAP 8
#endif // SB_INIT_CAP

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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
// Pushes a variadic amount of null-terminated strings to a string builder 
#define sb_push_str(sb, ...) sb_push_str_null(sb, __VA_ARGS__, NULL)

// Pushes a formatted string to a string builder
__attribute__((format(printf, 2, 3)))
void sb_push_sprintf(StringBuilder* sb, const char* fmt, ...);

// Pushes a sized string of certain length to a string builder 
#define sb_push_nstr(self, str, len) sb_push_bytes(self, str, len)

// Pushes bytes of certain length to a string builder 
void sb_push_bytes(StringBuilder* self, void* data, size_t sizeb);

// Writes the contents of a string builder to a file handle
#define sb_fwrite(sb, f) fwrite((sb)->items, 1, (sb)->count, f)
// Writes the contents of a string builder to a file
bool sb_write_file(StringBuilder* self, const char* filepath);

bool sb_fread(StringBuilder* sb, FILE* f, size_t n);

// Reads the provided file and appends its contents into the provided SB.
bool sb_read_file(StringBuilder* sb, const char* filepath); 

// Reads the provided file in batches and appends its contents into the provided SB.
bool sb_read_file_batches(StringBuilder* sb, const char* filepath);

// Exports the contents of a string builder to heap memory 
char* sb_export(StringBuilder const* sb);

// Exports the contents of a string builder to to provided DST 
void sb_export_inplace(StringBuilder const* sb, char dst[sb->count]);

// Frees a string builder
void sb_free(StringBuilder* sb);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STRING_BUILDER_H_

#ifdef SB_IMPLEMENTATION
#ifndef SB_MALLOC
    #define SB_MALLOC malloc
#endif // SB_MALLOC

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

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

void sb_push_bytes(StringBuilder* self, void* data, size_t sizeb) {
    sb_maybe_resize(self, sizeb);
    memcpy(self->items + self->count, data, sizeb);
    self->count += sizeb;
}

void sb_push_sprintf(StringBuilder* sb, const char* restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    va_start(args, fmt);

    sb_maybe_resize(sb, n + 1);
    int n_ = vsnprintf(sb->items + sb->count, n + 1, fmt, args);
    assert(n == n_);
    sb->count += n;

    va_end(args);
}

bool sb_read_file(StringBuilder* sb, const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", filepath, strerror(errno));
        return false;
    }

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

bool sb_fread(StringBuilder* sb, FILE* f, size_t n) {
    sb_maybe_resize(sb, n);

    errno = 0;
    size_t n_ = fread(sb->items + sb->count, 1, n, f);
    if (errno != 0) {
        fprintf(stderr, "Error reading file: %s\n", strerror(errno));
        return false;
    }

    sb->count += n;
    return n_ == n;
}

bool sb_write_file(StringBuilder* self, const char* filepath) {
    FILE* f = fopen(filepath, "wb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", filepath, strerror(errno));
        return false;
    }

    int n = sb_fwrite(self, f);
    assert(n == (int)self->count);

    fclose(f);

    return true;
}

bool sb_read_file_batches(StringBuilder* sb, const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", filepath, strerror(errno));
        return false;
    }

    char buf[1024] = {0};
    size_t n = 0;

    errno = 0;
    while ((n = fread(buf, 1, 1024, f)) > 0) {
        if (errno != 0) {
            fprintf(stderr, "Error reading file: %s\n", strerror(errno));
            fclose(f);
            return false;
        }

        sb_maybe_resize(sb, n);
        sb_push_nstr(sb, buf, n);

        errno = 0;
    }

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

