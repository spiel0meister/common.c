#ifndef STR_H_
#define STR_H_
#include <stddef.h>

#ifndef STR_MALLOC
#include <stdlib.h>
#define STR_MALLOC malloc
#endif // STR_MALLOC

#ifndef STR_FREE
#include <stdlib.h>
#define STR_FREE free
#endif // STR_FREE

typedef struct {
    size_t len;
    char chars[];
}string;

#define STR_FMT "%.*s"
#define STR_F(str) (int)(str)->len, (str)->chars

string* string_from_cstr(const char* str);
string* string_with_len(size_t len);
string* stringf(const char* fmt, ...);

string* string_copy(string* self);

#define string_free STR_FREE

#endif // STR_H

#ifdef STR_IMPLEMENTATION
#include <string.h>
#include <assert.h>

string* string_with_len(size_t len) {
    string* out = STR_MALLOC(sizeof(out->len) + sizeof(out->chars[0]) * len);
    assert(out != NULL);
    out->len = len;
    return out;
}

string* string_from_cstr(const char* str) {
    size_t len = strlen(str);
    string* out = string_with_len(len);
    if (out != NULL) {
        memcpy(out->chars, str, sizeof(out->chars[0]) * len);
    }
    return out;
}

string* stringf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    string* out = string_with_len(n - 1);

    va_start(args, fmt);
    int n_ = vsnprintf(out->chars, n, fmt, args);
    assert(n == n_);
    va_end(args);

    return out;
}

string* string_copy(string* self) {
    string* copy = string_with_len(self->len);
    memcpy(copy->chars, self->chars, sizeof(self->chars[0]) * self->len);
    return copy;
}

#endif // STR_IMPLEMENTATION
