#ifndef BSTRING_H
#define BSTRING_H
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    const char* start;
    const char* end;
}StringView;

#define cstr_to_view(cstr) ((StringView) { cstr, (cstr) + strlen(cstr) })

#ifndef TEXTBUF_LEN
    #define TEXTBUF_LEN 1024
#endif 
typedef struct {
    char buf[TEXTBUF_LEN];
}ShortString;

#define short_cstr(s) (s).buf

__attribute__ ((format(printf, 1, 2))) ShortString shortf(char const* fmt, ...);

bool str_endswith(const char* str, const char* cstr);
bool str_startswith(const char* str, const char* cstr);

ShortString char_repeat(char c, size_t repeat);

size_t string_view_len(StringView view);

#endif // BSTRING_H

#ifdef BSTRING_IMPLEMENTATION
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

size_t string_view_len(StringView view) {
    return view.end - view.start;
}

ShortString char_repeat(char c, size_t repeat) {
    ShortString s = {};
    for (int i = 0; i < TEXTBUF_LEN - 1 && i < repeat; ++i) {
        s.buf[i] = c;
    }
    return s;
}

ShortString shortf(char const* fmt, ...) {
    char buf[TEXTBUF_LEN] = {};
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

    ShortString out = {0};
    memcpy(out.buf, buf, size);
    out.buf[size] = 0;
    return out;
}

bool str_endswith(const char* str, const char* cstr) {
    for (int i = strlen(str) - 1, j = strlen(cstr) - 1; i >= 0 && j >= 0; --i) {
        if (str[i] != cstr[j--]) return false;
    }

    return true;
}

bool str_startswith(const char* str, const char* cstr) {
    for (int i = 0, j = 0; i < (int)strlen(str); ++i) {
        if (str[i] != cstr[j++]) return false;
    }

    return true;
}

#endif // BSTRING_IMPLEMENTATION
