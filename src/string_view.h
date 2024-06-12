#ifndef STIRNG_VIEW
#define STIRNG_VIEW
#endif // STIRNG_VIEW

#include <stddef.h>
#include <stdbool.h>

#define SV_FMT "%.*s"
#define SV_F(sv) (int)(sv).len, (sv).start

typedef bool (*predicate_t)(char c);

typedef struct {
    const char* start;
    size_t len;
}StringView;

// Creates a StringView from a C string
StringView sv_from_cstr(const char* str);

StringView sv_chop_by_c(StringView* sv, char delim);
StringView sv_trim_left(StringView sv);
StringView sv_trim_right(StringView sv);

StringView sv_chop_by_pred(StringView* sv, predicate_t pred);
StringView sv_trim_left_pred(StringView sv, predicate_t pred);
StringView sv_trim_right_pred(StringView sv, predicate_t pred);

#ifdef STRING_VIEW_IMPLEMENTATION
#include <string.h>
#include <ctype.h>

StringView sv_from_cstr(const char* str) {
    return (StringView) { .start = str, .len = strlen(str) };
}

StringView sv_chop_by_c(StringView* sv, char delim) {
    StringView out = { NULL, 0 };

    for (size_t i = 0; i < sv->len; ++i) {
        if (sv->start[i] == delim) {
            const char* frag_end = &sv->start[i + 1]; 

            out.start = sv->start;
            out.len = frag_end - sv->start;

            sv->start = frag_end;
            sv->len = sv->len - i - 1;

            break;
        }
    }
    
    if (out.start == NULL) {
        out.start = sv->start;
        out.len = sv->len;

        sv->start = &sv->start[sv->len - 1];
        sv->len = 0;
    }

    return out;
}

StringView sv_trim_left(StringView sv) {
    while (isspace(sv.start[0])) {
        ++sv.start;
        --sv.len;
    }

    return sv;
}

StringView sv_trim_right(StringView sv) {
    while (isspace(sv.start[sv.len - 1])) {
        --sv.len;
    }

    return sv;
}

StringView sv_chop_by_pred(StringView* sv, predicate_t pred) {
    StringView out = { NULL, 0 };

    for (size_t i = 0; i < sv->len; ++i) {
        if (pred(sv->start[i])) {
            const char* frag_end = &sv->start[i + 1]; 

            out.start = sv->start;
            out.len = frag_end - sv->start;

            sv->start = frag_end;
            sv->len = sv->len - i - 1;

            break;
        }
    }
    
    if (out.start == NULL) {
        out.start = sv->start;
        out.len = sv->len;

        sv->start = &sv->start[sv->len - 1];
        sv->len = 0;
    }

    return out;
}

StringView sv_trim_left_pred(StringView sv, predicate_t pred) {
    while (pred(sv.start[0])) {
        ++sv.start;
        --sv.len;
    }

    return sv;
}

StringView sv_trim_right_pred(StringView sv, predicate_t pred) {
    while (pred(sv.start[sv.len - 1])) {
        --sv.len;
    }

    return sv;
}

#endif // STRING_VIEW_IMPLEMENTATION
