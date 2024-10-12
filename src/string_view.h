#ifndef STRING_VIEW_H_
#define STRING_VIEW_H_

#include <stddef.h>
#include <stdbool.h>

#define SV_FMT "%.*s"
#define SV_F(sv) (int)(sv).len, (sv).start

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef bool (*sv_predicate_t)(char c);

typedef struct {
    const char* start;
    size_t len;
}StringView;

typedef struct {
    StringView* items;
    size_t count;
    size_t capacity;
}StringSplit;

// Creates a StringView from a C string
StringView sv_from_parts(const char* start, size_t len);
#define sv_from_cstr(str) sv_from_parts(str, strlen(str))

void sv_step(StringView* self);

StringView sv_chop_by_c(StringView* sv, char delim);
StringView sv_trim_left(StringView sv);
StringView sv_trim_right(StringView sv);
StringView sv_trim(StringView sv);

StringView sv_chop_by_pred(StringView* sv, sv_predicate_t pred);
StringView sv_trim_left_pred(StringView sv, sv_predicate_t pred);
StringView sv_trim_right_pred(StringView sv, sv_predicate_t pred);
StringView sv_trim_pred(StringView sv, sv_predicate_t pred);

long sv_to_long(StringView* sv, int base);
size_t sv_to_ulong(StringView* sv, int base);
float sv_to_float(StringView* sv);
double sv_to_double(StringView* sv);
long long sv_to_longlong(StringView* sv, int base);
unsigned long long sv_to_ulonglong(StringView* sv, int base);

StringSplit sv_split(StringView sv, char c);
StringSplit sv_split_pred(StringView sv, sv_predicate_t pred);

bool sv_cmpc(StringView sv, const char* cstr);
bool sv_cmpsv(StringView sv, StringView that);

char* sv_to_cstr(StringView sv);
char* sv_to_cstr_inplace(StringView sv, char buf[sv.len + 1]);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STRING_VIEW_H_

#ifdef SV_IMPLEMENTATION
#undef SV_IMPLEMENTATION

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef SV_MALLOC
#define SV_MALLOC malloc
#endif // SV_MALLOC

static void split_maybe_resize(StringSplit* split) {
    if (split->count == split->capacity) {
        split->capacity = split->capacity == 0? 2 : split->capacity * 2;
        split->items = realloc(split->items, sizeof(*split->items) * split->capacity);
        assert(split->items != NULL);
    }
}

static void split_append(StringSplit* split, StringView sv) {
    split_maybe_resize(split);
    split->items[split->count++] = sv;
}

StringView sv_from_parts(const char* start, size_t len) {
    return (StringView) { .start = start, .len = len };
}

void sv_step(StringView* self) {
    self->start += 1;
    self->len -= 1;
}

StringView sv_chop_by_c(StringView* sv, char delim) {
    StringView out = { NULL, 0 };

    for (size_t i = 0; i < sv->len; ++i) {
        if (sv->start[i] == delim) {
            const char* frag_end = &sv->start[i + 1]; 

            out.start = sv->start;
            out.len = frag_end - sv->start - 1;

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
    while (sv.len > 0 && isspace(sv.start[0])) {
        ++sv.start;
        --sv.len;
    }

    return sv;
}

StringView sv_trim_right(StringView sv) {
    while (sv.len > 0 && isspace(sv.start[sv.len - 1])) {
        --sv.len;
    }

    return sv;
}

StringView sv_trim(StringView sv) {
    return sv_trim_left(sv_trim_right(sv));
}

long sv_to_long(StringView* sv, int base) {
    char* end = NULL;
    long num = strtol(sv->start, &end, base);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

size_t sv_to_ulong(StringView* sv, int base) {
    char* end = NULL;
    size_t num = strtoul(sv->start, &end, base);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

double sv_to_double(StringView* sv) {
    char* end = NULL;
    double num = strtod(sv->start, &end);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

float sv_to_float(StringView* sv) {
    char* end = NULL;
    float num = strtof(sv->start, &end);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

unsigned long long sv_to_ulonglong(StringView* sv, int base) {
    char* end = NULL;
    unsigned long long num = strtoull(sv->start, &end, base);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

long long sv_to_longlong(StringView* sv, int base) {
    char* end = NULL;
    long long num = strtoll(sv->start, &end, base);
    sv->len -= end - sv->start;
    sv->start = end;
    return num;
}

StringSplit sv_split(StringView sv, char c) {
    StringSplit split = {};   
    while (sv.len > 0) {
        split_append(&split, sv_chop_by_c(&sv, c));
    }
    return split;
}

StringView sv_chop_by_pred(StringView* sv, sv_predicate_t pred) {
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

StringView sv_trim_left_pred(StringView sv, sv_predicate_t pred) {
    while (pred(sv.start[0])) {
        ++sv.start;
        --sv.len;
    }

    return sv;
}

StringView sv_trim_right_pred(StringView sv, sv_predicate_t pred) {
    while (pred(sv.start[sv.len - 1])) {
        --sv.len;
    }

    return sv;
}

StringView sv_trim_pred(StringView sv, sv_predicate_t pred) {
    return sv_trim_left_pred(sv_trim_right_pred(sv, pred), pred);
}

StringSplit sv_split_pred(StringView sv, sv_predicate_t pred) {
    StringSplit split = {};   
    while (sv.len > 0) {
        split_append(&split, sv_chop_by_pred(&sv, pred));
    }
    return split;
}

bool sv_cmpc(StringView sv, const char* cstr) {
    if (sv.len != strlen(cstr)) return false;

    for (size_t i = 0; i < sv.len; ++i) {
        if (sv.start[i] != cstr[i]) return false;
    }

    return true;
}

bool sv_cmpsv(StringView sv, StringView that) {
    if (sv.len != that.len) return false;

    for (size_t i = 0; i < sv.len; ++i) {
        if (sv.start[i] != that.start[i]) return false;
    }

    return true;
}

char* sv_to_cstr(StringView sv) {
    char* buf = SV_MALLOC(sv.len + 1);
    memcpy(buf, sv.start, sv.len);
    buf[sv.len] = 0;
    return buf;
}

char* sv_to_cstr_inplace(StringView sv, char buf[sv.len + 1]) {
    memcpy(buf, sv.start, sv.len);
    buf[sv.len] = 0;
    return buf;
}


#endif // STRING_VIEW_IMPLEMENTATION
