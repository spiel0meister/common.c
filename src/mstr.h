#ifndef MODERN_STRING_H_
#define MODERN_STRING_H_
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    size_t len;
    size_t cap;
    char* buffer;
}String;

typedef struct {
    size_t len;
    char* start;
}str;

#define STR_FMT "%.*s"
#define STR_F(str) (int)(str).len, (str).start

#define str_from_parts(start_, len_) ((str) { .start = (start_), .len = (len_) })
#define str_from_cstr(cstr) str_from_parts(cstr, strlen(cstr))
#define str_from_string(string) str_from_parts((string).start, (string).len)

void string_maybe_resize(String* string, size_t to_add);
#define string_append_char(string, c) (string_maybe_resize(string, 1), (string)->buffer[(string)->len++] = (c))
#define string_append_null(string) string_append_char(string, 0)
void string_append(String* string, str str);
void string_appendf(String* string, const char* fmt, ...);

bool string_read_entire_file(String* self, const char* file);
bool string_write_entire_file(String* self, const char* file);

str str_substr(str self, size_t i);
bool str_compare(str self, str that);
str str_trim_left(str str);
str str_trim_right(str str);
#define str_trim(str) str_trim_left(str_trim_right(str))

str str_chop_c(str* self, char c);

uint64_t str_to_u64(str str, size_t base);
double str_to_double(str str);
uint64_t str_chop_u64(str* str, size_t base);
double str_chop_double(str* str);

#endif // MODERN_STRING_H_

#ifdef MODERN_STRING_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

static str empty_str = { .start = NULL, .len = 0 };

void string_maybe_resize(String* string, size_t to_add) {
    if (string->len + to_add <= string->cap) {
        if (string->cap == 0) string->cap = 16;
        while (string->len + to_add <= string->cap) string->cap *= 2;
        string->buffer = realloc(string->buffer, string->cap);
    }
}

void string_append(String* string, str str) {
    string_maybe_resize(string, str.len);
    memcpy(string->buffer + string->len, str.start, str.len);
    string->len += str.len;
}

void string_appendf(String* string, const char* fmt, ...) {
static char buffer[1024] = {0};

    va_list args;
    va_start(args, fmt);

    int len = vsprintf(buffer, fmt, args);
    assert(len >= 0);

    str to_append = str_from_parts(buffer, len);
    string_append(string, to_append);

    va_end(args);
}

str str_substr(str self, size_t i) {
    if (i >= self.len) return empty_str;
    return str_from_parts(self.start + i, self.len - i);
}

str str_chop_c(str* self, char c) {
    size_t i = 0;
    while (i < self->len && self->start[i] != c) {
        ++i;
    }

    str result = str_from_parts(self->start, i);

    self->start += i;
    self->len -= i;

    if (i < self->len) {
        self->start += 1;
        self->len -= 1;
    }

    return result;
}

uint64_t str_to_u64(str str, size_t base) {
    char* end;
    uint64_t ret = strtoul(str.start, &end, base);
    return ret;
}

uint64_t str_chop_u64(str* str, size_t base) {
    char* end;
    uint64_t ret = strtoul(str->start, &end, base);
    str->len = end - str->start;
    str->start = end;
    return ret;
}

double str_to_double(str str) {
    char* end;
    double ret = strtod(str.start, &end);
    return ret;
}

double str_chop_double(str* str) {
    char* end;
    double ret = strtod(str->start, &end);
    str->len = end - str->start;
    str->start = end;
    return ret;
}

str str_trim_left(str str) {
    while (str.len > 0 && isalpha(*str.start)) {
        str.start++;
        str.len--;
    }
    return str;
}

str str_trim_right(str str) {
    while (str.len > 0 && isalpha(str.start[str.len - 1])) {
        str.len--;
    }
    return str;
}

bool str_compare(str self, str that) {
    size_t len = self.len;
    if (len > that.len) len = that.len;
    return memcmp(self.start, that.start, len) == 0;
}

bool string_read_entire_file(String* self, const char* file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", file, strerror(errno));
        return false;
    }

    fseek(f, 0, SEEK_END);
    int n = ftell(f);
    fseek(f, 0, SEEK_SET);

    fread(self->buffer + self->len, n, 1, f);
    self->len += n;

    fclose(f);
    return true;
}

bool string_write_entire_file(String* self, const char* file) {
    FILE* f = fopen(file, "wb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", file, strerror(errno));
        return false;
    }

    fwrite(self->buffer, 1, self->len, f);
    fclose(f);
    return true;
}

#endif // MODERN_STRING_IMPLEMENTATION
