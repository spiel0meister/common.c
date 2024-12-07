#ifndef ARENA_H_
#define ARENA_H_
#include <stdint.h>
#include <stdlib.h>

#ifndef REGION_DEFAULT_SIZE
#define REGION_DEFAULT_SIZE (1 << 10)
#endif // REGION_DEFAULT_SIZE

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC malloc
#endif // ARENA_MALLOC 

#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE free
#endif // ARENA_FREE 

typedef struct arena_region_s {
    size_t capacity;
    size_t count;
    struct arena_region_s* next;

    uintptr_t data[];
}ArenaRegion;

typedef struct {
    ArenaRegion *start, *end;
}Arena;

typedef struct {
    ArenaRegion* r;
    size_t count;
}ArenaMark;

void* arena_alloc(Arena* self, size_t size);
#define arena_calloc(arena, size) memset(arena_alloc(arena, size), 0, size)
void* arena_realloc(Arena* self, size_t oldsize, size_t newsize, void* ptr);
#define arena_memdup(arena, ptr, size) memcpy(arena_alloc(arena, size), ptr, size)
char* arena_sprintf(Arena* self, const char* fmt, ...);
char* arena_strdup(Arena* self, const char* cstr);
char* arena_realpath(Arena* self, const char* path);

ArenaMark arena_mark(Arena* self);
void arena_jumpback(Arena* self, ArenaMark mark);
void arena_reset(Arena* self);
void arena_free(Arena* self);

#endif // ARENA_H_

#ifdef ARENA_IMPLEMENTATION
#undef ARENA_IMPLEMENTATION

#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

void* arena_alloc(Arena* self, size_t size) {
    if (self->end == NULL) {
        self->end = ARENA_MALLOC(sizeof(*self->start) + REGION_DEFAULT_SIZE * sizeof(*self->start->data));
        self->end->count = 0;
        self->end->capacity = REGION_DEFAULT_SIZE;
        self->end->next = NULL;
        self->start = self->end;
    }
    assert(self->end != NULL);

    while (self->end->count + size > self->end->capacity && self->end->next != NULL) {
        self->end = self->end->next;
    }

    if (self->end->count + size > self->end->capacity) {
        self->end->next = ARENA_MALLOC(sizeof(*self->end->next) + REGION_DEFAULT_SIZE * sizeof(*self->start->data));
        self->end->next->count = 0;
        self->end->next->capacity = REGION_DEFAULT_SIZE;
        self->end->next->next = NULL;
        self->end = self->end->next;
    }

    size_t word_size = sizeof(uintptr_t);
    size_t realsize = (size + word_size - 1) / word_size;

    void* mem = self->end->data + self->end->count;
    self->end->count += realsize;
    return mem;
}

void* arena_realloc(Arena* self, size_t oldsize, size_t newsize, void* ptr) {
    void* newptr = arena_alloc(self, newsize);
    return memcpy(newptr, ptr, oldsize);
}

char* arena_sprintf(Arena* self, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

#pragma GCC diagnostic ignored "-Wformat-truncation"
    int n = vsnprintf(NULL, 0, fmt, args);

    va_end(args);
    
    char* cstr = arena_alloc(self, n);

    va_start(args, fmt);

    int n_ = vsnprintf(NULL, n + 1, fmt, args);
    assert(n_ == n);

    va_end(args);

    return cstr;
}

char* arena_strdup(Arena* self, const char* cstr) {
    size_t len = strlen(cstr) + 1;
    char* copy = arena_alloc(self, len);
    return memcpy(copy, cstr, len);
}

char* arena_realpath(Arena* self, const char* path) {
    char* real = realpath(path, NULL);
    if (real == NULL) {
        fprintf(stderr, "Couldn't get real path '%s': %s\n", path, strerror(errno));
        return NULL;
    }

    char* out = arena_strdup(self, real);
    free(real);
    return out;
}

void arena_reset(Arena* self) {
    for (ArenaRegion* r = self->start; r != NULL; r = r->next) {
        r->count = 0;
    }

    self->end = self->start;
}

ArenaMark arena_mark(Arena* self) {
    ArenaMark mark = {self->end, self->end->count};
    return mark;
}

void arena_jumpback(Arena* self, ArenaMark mark) {
    if (mark.r == NULL) {
        arena_reset(self);
        return;
    }

    mark.r->count = mark.count;
    for (ArenaRegion* r = mark.r; r != NULL; r = r->next) {
        r->count = 0;
    }

    self->end = mark.r;
}

void arena_free(Arena* self) {
    ArenaRegion* r = self->start;
    while (r != NULL) {
        ArenaRegion* n = r->next;
        ARENA_FREE(r);
        r = n;
    }
    self->start = NULL;
    self->end = NULL;
}
#endif // ARENA_IMPLEMENTATION
#ifndef PERF_H_
#define PERF_H_
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

double clock_secs(clock_t start);
double clock_msecs(clock_t start);
long clock_nsecs(clock_t start);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PERF_H_

#ifdef CPERF_IMPLEMENTATION
#undef CPERF_IMPLEMENTATION

double clock_secs(clock_t start) {
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

double clock_msecs(clock_t start) {
    return clock_secs(start) * 1000;
}

long clock_nsecs(clock_t start) {
    return clock_secs(start) * 1000000;
}

#endif
#ifndef DAH_H_
#define DAH_H_
#endif // DAH_H_

#include <stddef.h>
#include <stdint.h>

#ifndef DAH_MALLOC
#include <stdlib.h>
#define DAH_MALLOC malloc
#endif // DAH_MALLOC

#ifndef DAH_REALLOC
#include <stdlib.h>
#define DAH_REALLOC realloc
#endif // DAH_REALLOC

typedef struct {
    size_t count, capacity;
    uint8_t data[];
}DaHeader;

#define DAH_INIT_CAPACITY 16

#define dah_init(da) ((da) = (void*)dah__default_header(sizeof(Statement))->data)

#define dah_append(da, v) ((da) = dah__maybe_resize(da, 1, sizeof(*(da))), (da)[dah_getheader(da)->count++] = (v))
#define dah_append_many(da, vs, vcount) ((da) = dah__maybe_resize(da, vcount, sizeof(*(da))), memcpy((da) + dah_getheader(da)->count, vs, vcount * sizeof(*(da))), dah_getheader(da)->count += vcount)
#define dah_append_cstr(da, cstr) dah_append_many(da, cstr, strlen(cstr))
#define dah_remove_unordered(da, i) ((da)[i] = (da)[--dah_getheader(da)->count])
#define dah_remove_ordered(da, i) dah__remove_ordered(da, i, sizeof(*(da)))
#define dah_reset(da) (da_getheader(da)->count = 0)
#define dah_free(da) ((da) = dah__free(da))

#define dah_for(da, counter) for (size_t counter = 0; counter < dah_getheader(da)->count; ++counter)
#define dah_foreach(da, Type, ptr) for (Type* ptr = da; ptr < da + dah_getheader(da)->count; ++ptr)

DaHeader* dah_getheader(void* da);
size_t dah_getlen(void* da);

DaHeader* dah__default_header(size_t item_size);
void* dah__maybe_resize(void* da, size_t to_add, size_t item_size);
void dah__remove_ordered(void* da, size_t i, size_t item_size);
void* dah__free(void* da);

#ifdef DAH_IMPLEMENTATION
#undef DAH_IMPLEMENTATION

#include <string.h>

DaHeader* dah__default_header(size_t item_size) {
    DaHeader* header = DAH_MALLOC(sizeof(DaHeader) + item_size * DAH_INIT_CAPACITY);
    header->count = 0;
    header->capacity = DAH_INIT_CAPACITY;
    return header;
}

void* dah__maybe_resize(void* da, size_t to_add, size_t item_size) {
    if (da == NULL) {
        return dah__default_header(item_size)->data;
    }

    DaHeader* header = (DaHeader*)da - 1;
    
    if (header->count + to_add >= header->capacity) {
        if (header->capacity == 0) header->capacity = 2;
        while (header->count + to_add >= header->capacity) header->capacity *= 2;
        header = DAH_REALLOC(header, sizeof(DaHeader) + header->capacity * item_size);
    }

    return header->data;
}

size_t dah_getlen(void* da) {
    if (da == NULL) return 0;
    return dah_getheader(da)->count;
}

DaHeader* dah_getheader(void* da) {
    return (DaHeader*)da - 1;
}

void dah__remove_ordered(void* da, size_t i, size_t item_size) {
    DaHeader* header = dah_getheader(da);

    memmove(header->data + i * item_size, header->data + (i + 1) * item_size, (header->count - i - 1) * item_size);
    header->count--;
}

void* dah__free(void* da) {
    free(dah_getheader(da));
    return NULL;
}

#endif
#ifndef EASINGS_H_
#define EASINGS_H_
#include <math.h>

#ifndef PI
#define PI 3.14159 
#endif // PI

#ifndef EASINGS_POWF
#include <math.h>
#define EASINGS_POWF powf
#endif

#ifndef EASINGS_SQRTF
#include <math.h>
#define EASINGS_SQRTF sqrtf
#endif

#ifndef EASINGS_EXP2F
#include <math.h>
#define EASINGS_EXP2F exp2f
#endif

float ease_in_sine(float t);
float ease_out_sine(float t);
float ease_in_out_sine(float t);

float ease_in_quad(float t);
float ease_out_quad(float t);
float ease_in_out_quad(float t);

float ease_in_cubic(float t);
float ease_out_cubic(float t);
float ease_in_out_cubic(float t);

float ease_in_quart(float t);
float ease_out_quart(float t);
float ease_in_out_quart(float t);

float ease_in_quint(float t);
float ease_out_quint(float t);
float ease_in_out_quint(float t);

float ease_in_expo(float t);
float ease_out_expo(float t);
float ease_in_out_expo(float t);

float ease_in_circ(float t);
float ease_out_circ(float t);
float ease_in_out_circ(float t);

float ease_in_back(float t);
float ease_out_back(float t);
float ease_in_out_back(float t);

float ease_in_elastic(float t);
float ease_out_elastic(float t);
float ease_in_out_elastic(float t);

float ease_in_bounce(float t);
float ease_out_bounce(float t);
float ease_in_out_bounce(float t);

#endif // EASINGS_H_

#ifdef EASINGS_IMPLEMENTATION
#undef EASINGS_IMPLEMENTATION

float ease_in_sine(float t) {
    return 1 - cos((t * PI) / 2);
}

float ease_out_sine(float t) {
    return sin((t * PI) / 2);
}

float ease_in_out_sine(float t) {
    return -(cos(PI * t) - 1) / 2;
}

float ease_in_quad(float t) {
    return t * t;
}

float ease_out_quad(float t) {
    return 1 - (1 - t) * (1 - t);
}

float ease_in_out_quad(float t) {
    return t < 0.5 ? 2 * t * t : 1 - EASINGS_POWF(-2 * t + 2, 2) / 2;
}

float ease_in_cubic(float t) {
    return t * t * t;
}

float ease_out_cubic(float t) {
    return 1 - EASINGS_POWF(1 - t, 3);
}

float ease_in_out_cubic(float t) {
    return t < 0.5 ? 4 * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 3) / 2;
}

float ease_in_quart(float t) {
    return t * t * t * t;
}

float ease_out_quart(float t) {
    return 1 - EASINGS_POWF(1 - t, 4);
}

float ease_in_out_quart(float t) {
    return t < 0.5 ? 8 * t * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 4) / 2;
}

float ease_in_quint(float t) {
    return t * t * t * t * t;
}

float ease_out_quint(float t) {
    return 1 - EASINGS_POWF(1 - t, 5);
}

float ease_in_out_quint(float t) {
    return t < 0.5 ? 16 * t * t * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 5) / 2;
}

float ease_in_expo(float t) {
    return t == 0.0f ? 0.0f : EASINGS_EXP2F(10 * t - 10);
}

float ease_out_expo(float t) {
    return t == 1.0f ? 1.0f : 1 - EASINGS_EXP2F(-10 * t);
}

float ease_in_out_expo(float t) {
    if (t == 0) return 0;
    else if (t == 1) return 1;
    else if (t < 0.5) return EASINGS_EXP2F(20 * t - 10) / 2;
    else return (2 - EASINGS_EXP2F(-20 * t + 10)) / 2;
}

float ease_in_circ(float t) {
    return 1 - EASINGS_SQRTF(1 - t * t);
}

float ease_out_circ(float t) {
    return EASINGS_SQRTF(1 - EASINGS_POWF(t - 1, 2));
}

float ease_in_out_circ(float t) {
    if (t < 0.5) return (1 - EASINGS_SQRTF(1 - EASINGS_POWF(2 * t, 2))) / 2;
    else return (EASINGS_SQRTF(1 - EASINGS_POWF(-2 * t + 2, 2)) + 1) / 2;
}

float ease_in_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return c3 * t * t * t - c1 * t * t;
}

float ease_out_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return 1 + c3 * EASINGS_POWF(t - 1, 3) + c1 * EASINGS_POWF(t - 1, 2);
}

float ease_in_out_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;

    if (t < 0.5) return (EASINGS_POWF(2 * t, 2) * ((c3 + 1) * 2 * t - c3)) / 2;
    else return (EASINGS_POWF(2 * t - 2, 2) * ((c3 + 1) * (t * 2 - 2) + c3) + 2) / 2;
}

float ease_in_elastic(float t) {
    float c1 = (2 * PI) / 3;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : -EASINGS_POWF(2, 10 * t - 10) * sinf((t * 10 - 10.75) * c1);
}

float ease_out_elastic(float t) {
    float c1 = (2 * PI) / 3;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : EASINGS_POWF(2, -10 * t) * sinf((t * 10 - 0.75) * c1) + 1;
}

float ease_in_out_elastic(float t) {
    float c1 = (2 * PI) / 4.5;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : t < 0.5
        ? -(EASINGS_POWF(2, 20 * t - 10) * sinf((20 * t - 11.125) * c1)) / 2
        : (EASINGS_POWF(2, -20 * t + 10) * sinf((20 * t - 11.125) * c1)) / 2 + 1;
}

float ease_in_bounce(float t) {
    return 1 - ease_out_bounce(1 - t);
}

float ease_out_bounce(float t) {
    float n1 = 7.5625;
    float d1 = 2.75;

    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    } else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    } else {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}


float ease_in_out_bounce(float t) {
    return t < 0.5
        ? (1 - ease_out_bounce(1 - 2 * t)) / 2
        : (1 + ease_out_bounce(2 * t - 1)) / 2;
}

#endif // EASINGS_IMPLEMENTATION
#ifndef FLAG_H_
#define FLAG_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define FLAG_TYPE_LIST \
    X(FLAG_OPTION, bool, option, flag_option) \
    X(FLAG_STR, char*, cstr, flag_cstr) \
    X(FLAG_INT, int, i, flag_int) \
    X(FLAG_FLOAT, float, f, flag_float) \
    X(FLAG_LONG, long, l, flag_long) \
    X(FLAG_LLONG, long long, ll, flag_llong)

#define X(flag_type, _1, _2, _3) flag_type,
typedef enum {
    FLAG_TYPE_LIST
}FlagType;
#undef X

#define X(_1, Type, ufield_name, _3) Type ufield_name;
typedef union {
FLAG_TYPE_LIST
}FlagAs;
#undef X

typedef struct {
    FlagType type;
    const char* name;
    const char* desc;

    FlagAs as;
}Flag;

#ifndef FLAG_ARR_CAP
#define FLAG_ARR_CAP 128
#endif // FLAG_ARR_CAP

#define X(flag_type, Type, ufield_name, name_) Type* name_(const char* name, Type def, const char* desc);
FLAG_TYPE_LIST
#undef X

// Print help for flags
void flag_type_help(FILE* sink);

// Parse flags. Implicitly adds -h to print help. Returns from the index from which point on forward are positional arguments
int flag_parse(int argc, char** argv);

#endif // FLAG_H_

#ifdef FLAG_IMPLEMENTATION
#undef FLAG_IMPLEMENTATION

#include <string.h>
#include <assert.h>
#include <stdlib.h>

static Flag flags[FLAG_ARR_CAP] = {0};
static size_t flags_size = 0;

Flag* flag__new(FlagType type, const char* name, const char* desc) {
    assert(flags_size < FLAG_ARR_CAP);

    Flag* flag = &flags[flags_size++];
    flag->type = type;
    flag->name = name;
    flag->desc = desc;
    return flag;
}

#define X(flag_type, Type, ufield_name, name_) \
Type* name_(const char* name, Type def, const char* desc) { \
    Flag* opt = flag__new(flag_type, name, desc); \
    opt->as.ufield_name = def; \
    return &opt->as.ufield_name; \
}
FLAG_TYPE_LIST
#undef X

char flag_tmp_buf[128] = {0};
char* flag__tmp_sprintf(Flag* flag) {
    switch (flag->type) {
    case FLAG_OPTION:
        return flag->as.option ? "true" : "false";
    case FLAG_STR:
        snprintf(flag_tmp_buf, sizeof(flag_tmp_buf), "\"%s\"", flag->as.cstr);
        return flag_tmp_buf;
    case FLAG_INT:
        snprintf(flag_tmp_buf, sizeof(flag_tmp_buf), "%d", flag->as.i);
        return flag_tmp_buf;
    case FLAG_FLOAT:
        snprintf(flag_tmp_buf, sizeof(flag_tmp_buf), "%f", flag->as.f);
        return flag_tmp_buf;
    case FLAG_LONG:
        snprintf(flag_tmp_buf, sizeof(flag_tmp_buf), "%ld", flag->as.l);
        return flag_tmp_buf;
    case FLAG_LLONG:
        snprintf(flag_tmp_buf, sizeof(flag_tmp_buf), "%lld", flag->as.ll);
        return flag_tmp_buf;
    }

    assert(0 && "unreachable");
}

char* flag__shift_args(int* argc, char*** argv) {
    assert(*argc > 0);

    char* ret = **argv;
    (*argv) += 1;
    (*argc) -= 1;
    return ret;
}

void flag_type_help(FILE* sink) {
    fprintf(sink, "Flags:\n");
    for (size_t i = 0; i < flags_size; ++i) {
        fprintf(sink, "    -%s\n", flags[i].name);
        fprintf(sink, "        %s (Default: %s)\n", flags[i].desc, flag__tmp_sprintf(&flags[i]));
    }
}

int flag_parse(int argc, char** argv) {
    int saved_argc = argc;

    char* _program = flag__shift_args(&argc, &argv);
    (void)_program;

    while (argc > 0) {
        char* arg = flag__shift_args(&argc, &argv);

        if (arg[0] == '-') {
            arg += 1;

            if (strcmp(arg, "h") == 0) {
                flag_type_help(stdout);
                exit(0);
            }

            bool found = false;
            for (size_t i = 0; i < flags_size; ++i) {
                if (strcmp(flags[i].name, arg) == 0) {
                    found = true;
                    switch (flags[i].type) {
                    case FLAG_OPTION:
                        flags[i].as.option = true;
                        break;
                    case FLAG_STR:
                        if (argc < 1) {
                            fprintf(stderr, "ERROR: Missing argument for flag -%s\n", arg);
                            exit(1);
                        }

                        flags[i].as.cstr = flag__shift_args(&argc, &argv);
                        break;
                    case FLAG_INT:
                        if (argc < 1) {
                            fprintf(stderr, "ERROR: Missing argument for flag -%s\n", arg);
                            exit(1);
                        }

                        flags[i].as.i = atoi(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_FLOAT:
                        if (argc < 1) {
                            fprintf(stderr, "ERROR: Missing argument for flag -%s\n", arg);
                            exit(1);
                        }

                        flags[i].as.f = atof(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_LONG:
                        if (argc < 1) {
                            fprintf(stderr, "ERROR: Missing argument for flag -%s\n", arg);
                            exit(1);
                        }

                        flags[i].as.l = atol(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_LLONG:
                        if (argc < 1) {
                            fprintf(stderr, "ERROR: Missing argument for flag -%s\n", arg);
                            exit(1);
                        }

                        flags[i].as.ll = atoll(flag__shift_args(&argc, &argv));
                        break;
                    default:
                        assert(0 && "unreachable");
                        break;
                    }
                }
            }

            if (!found) {
                fprintf(stderr, "ERROR: Unknown flag -%s\n", arg);
                exit(1);
            }
        } else {
            return saved_argc - argc;
        }
    }

    return saved_argc - argc;
}


#endif
#ifndef FS_H_
#define FS_H_
#include <stdbool.h>

char* read_entire_file(const char* path);
bool write_to_file(const char* path, const char* content, size_t n);

#endif // FS_H_

#ifdef FS_IMPLEMENTATION
#undef FS_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#error "TODO: Windows"
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _WIN32

char* read_entire_file(const char* path) {
    bool result = true;
    char* content = NULL;

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Couldn't open %s: %s\n", path, strerror(errno));
        result = false;
        goto defer;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stderr, "Couldn't stat %s: %s\n", path, strerror(errno));
        result = false;
        goto defer;
    }

    content = malloc(file_stat.st_size + 1);
    if (read(fd, content, file_stat.st_size) < 0) {
        fprintf(stderr, "Couldn't read %s: %s\n", path, strerror(errno));
        result = false;
        goto defer;
    }

defer:
    if (!result) {
        free(content);
        content = NULL;
    }
    if (fd > 0) close(fd);

    return content;
}

bool write_to_file(const char* path, const char* content, size_t n) {
    bool result = true;

    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        fprintf(stderr, "Couldn't open %s: %s\n", path, strerror(errno));
        result = false;
        goto defer;
    }

    if (write(fd, content, n) < 0) {
        fprintf(stderr, "Couldn't write to %s: %s\n", path, strerror(errno));
        result = false;
        goto defer;
    }

defer:
    if (fd > 0) close(fd);
    return result;
}

#endif // FS_IMPLEMENTATION
#ifndef LINEAR_H_
#define LINEAR_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef LINEAR_DEFAULT_CAPACITY
#define LINEAR_DEFAULT_CAPACITY (1 << 20)
#endif // LINEAR_DEFAULT_CAPACITY

#ifndef LINEAR_ASSERT
#include <assert.h>
#define LINEAR_ASSERT(expr) assert(expr)
#endif // LINEAR_ASSERT

#ifndef cast
#ifdef __cplusplus
#define cast(Type, thing) reinterpret_cast<Type>(thing)
#else // __cplusplus
#define cast(Type, thing) (Type)(thing)
#endif // __cplusplus
#endif // cast

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    size_t size;
    size_t capacity;
    uintptr_t* mem;
}Linear;

#define linear_alloc(linear, Type) (Type*)linear_allocb(linear, sizeof(Type))
#define linear_calloc(linear, Type) (Type*)linear_callocb(linear, sizeof(Type))

#define linear_alloc_array(linear, n, Type) (Type*)linear_allocb(linear, sizeof(Type) * (n))
#define linear_calloc_array(linear, n, Type) (Type*)linear_callocb(linear, sizeof(Type) * (n))

#define linear_occupied(linear) (LINEAR_ASSERT((linear) != NULL), (linear)->size*sizeof(uintptr_t))
#define linear_can_alloc(linear, Type) linear_can_allocb(linear, sizeof(Type))

Linear linear_new(size_t capacity);
void linear_prealloc(Linear* self, size_t capacity);
char* linear_strdup(Linear* self, const char* cstr);
char* linear_sprintf(Linear* self, const char* fmt, ...);

size_t linear_snapshot(Linear* self);
void linear_rewind(Linear* self, size_t offset);

void linear_free(Linear* self);
#define linear_reset(linear) ((linear)->size = 0)

void* linear_allocb(Linear* self, size_t size);
void* linear_callocb(Linear* self, size_t size);
void* linear_memdupb(Linear* self, void* mem, size_t size);
bool linear_can_allocb(Linear* self, size_t size);

extern Linear temp_linear;
#define temp_prealloc(size) linear_prealloc(&temp_linear, size)
#define temp_alloc(Type) linear_alloc(&temp_linear, Type)
#define temp_allocb(size) linear_allocb(&temp_linear, size)
#define temp_sprintf(...) linear_sprintf(&temp_linear, __VA_ARGS__)
#define temp_snapshot() linear_snapshot(&temp_linear)
#define temp_rewind(offset) linear_rewind(&temp_linear, offset)
#define temp_free() linear_free(&temp_linear)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LINEAR_H_

#ifdef LINEAR_IMPLEMENTATION
#undef LINEAR_IMPLEMENTATION

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

Linear temp_linear = {0};

Linear linear_new(size_t capacity) {
    Linear a = {0};
    linear_prealloc(&a, capacity);
    return a;
}

void linear_prealloc(Linear* self, size_t capacity) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_capacity = (capacity + word_size - 1)/word_size;

    self->mem = cast(uintptr_t*, malloc(real_capacity * word_size));
    self->size = 0;
    self->capacity = real_capacity;
}

size_t linear_snapshot(Linear* self) {
    return self->size;
}

void linear_rewind(Linear* self, size_t offset) {
    self->size = offset;
}

void* linear_allocb(Linear* self, size_t size) {
    if (self->mem == NULL) linear_prealloc(self, LINEAR_DEFAULT_CAPACITY);

    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;

    if (self->size + real_size > self->capacity) {
        return NULL;
    }

    size_t i = self->size;
    self->size += real_size;
    return cast(void*, self->mem + i);
}

void* linear_callocb(Linear* self, size_t size) {
    void* p = linear_allocb(self, size);
    memset(p, 0, size);
    return p;
}

void* linear_memdupb(Linear* self, void* mem, size_t size) {
    void* dup = linear_allocb(self, size);
    if (dup == NULL) return NULL;

    memcpy(dup, mem, size);
    return dup;
}

char* linear_strdup(Linear* self, const char* cstr) {
    size_t size = strlen(cstr);
    char* dup = linear_alloc_array(self, size, char);
    if (dup == NULL) return NULL;

    memcpy(dup, cstr, size);
    return dup;
}

char* linear_sprintf(Linear* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    va_start(args, fmt);
    char* cstr = linear_alloc_array(self, n + 1, char);
    vsnprintf(cstr, n + 1, fmt, args);
    va_end(args);

    return cstr;
}

size_t linear_get_offset(Linear* self, void* thing) {
    assert((uint8_t*)self->mem <= (uint8_t*)thing && (uint8_t*)thing < (uint8_t*)self->mem + self->capacity);

    return cast(char*, thing) - cast(char*, self->mem);
}

void* linear_exportb(Linear* self, size_t offset, size_t size) {
    void* mem = malloc(size);
    memcpy(mem, self->mem + offset, size);
    return mem;
}


bool linear_can_allocb(Linear* a, size_t size) {
    size_t word_size = sizeof(uintptr_t);
    size_t real_size = (size + word_size - 1)/word_size;
    return a->size + real_size < a->capacity;
}

void linear_free(Linear* self) {
    free(self->mem);
    self->mem = NULL;
    self->size = 0;
    self->capacity = 0;
}
#endif // LINEAR_IMPLEMENTATION

#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,

    LOG_COUNT_,
}LogLevel;

typedef struct {
    LogLevel level;
    FILE* log_file;
}Logger;

extern Logger global_logger;

void logger_log(Logger* self, LogLevel level, const char* fmt, ...);
#define logger_info(logger, ...) logger_log(logger, LOG_INFO, __VA_ARGS__)
#define logger_warn(logger, ...) logger_log(logger, LOG_WARN, __VA_ARGS__)
#define logger_debug(logger, ...) logger_log(logger, LOG_DEBUG, __VA_ARGS__)
#define logger_error(logger, ...) logger_log(logger, LOG_ERROR, __VA_ARGS__)

#endif // LOG_H_

#ifdef LOG_IMPLEMENTATION
#include <stdarg.h>
#include <assert.h>

Logger global_logger;

_Static_assert(LOG_COUNT_ == 4, "New log level was added");
const char* log_levels_as_str[] = {
    [LOG_DEBUG] = "DEBUG",
    [LOG_INFO] = "INFO",
    [LOG_WARN] = "WARN",
    [LOG_ERROR] = "ERROR",
};

void logger_log(Logger* self, LogLevel level, const char* fmt, ...) {
    if (self == &global_logger && self->log_file == NULL) self->log_file = stdout;
    if (self->level > level) return;

    assert(self->log_file != NULL && "log_file must not be NULL");

    va_list args;
    va_start(args, fmt);

    fprintf(self->log_file, "[%s] ", log_levels_as_str[level]);
    vfprintf(self->log_file, fmt, args);

    va_end(args);
}

#endif
#ifndef PROCESS_H_
#define PROCESS_H_
#endif // PROCESS_H_

#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#ifndef PROCESS_MALLOC
#include <stdlib.h>
#define PROCESS_MALLOC malloc
#endif // PROCESS_MALLOC

#ifndef PROCESS_FREE
#include <stdlib.h>
#define PROCESS_FREE free
#endif // PROCESS_FREE

#ifndef PROCESS_ASSERT
#include <assert.h>
#define PROCESS_ASSERT assert
#endif // PROCESS_ASSERT

#define PIPE_READ 0
#define PIPE_WRITE 1

bool process_close(int fd);
bool process_dup2(int fd1, int fd2);
bool process_pipe(int pipefd[2]);

bool process_wait_for_exit(pid_t pid);
int process_start(const char* args[], size_t args_count);

bool process_run(const char* args[], size_t args_count);

typedef struct {
    int pid;
    int in_pipe[2];
    int out_pipe[2];
}Proc;

#define PROCESS_PIPE_STDIN (1 << 0)
#define PROCESS_PIPE_STDOUT (1 << 1)
#define PROCESS_PIPE_STDERR (1 << 2)
#define PROCESS_PIPE_ALL (PROCESS_PIPE_STDIN | PROCESS_PIPE_STDOUT | PROCESS_PIPE_STDERR)

Proc process_start_ex(const char* args[], size_t args_count, uint8_t pipe_flags);
bool prcess_proc_destroy(Proc* proc);

#ifdef PROCESS_IMPLEMENTATION
#undef PROCESS_IMPLEMENTATION
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

bool process_close(int fd) {
    if (close(fd) < 0) {
        fprintf(stderr, "Couldn't close %d: %s\n", fd, strerror(errno));
        return false;
    }

    return true;
}

bool process_dup2(int fd1, int fd2) {
    if (dup2(fd1, fd2) < 0) {
        fprintf(stderr, "Couldn't dup2: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool process_pipe(int pipefd[2]) {
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool process_wait_for_exit(pid_t pid) {
    while (1) {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0) {
            fprintf(stderr, "Couldn't wait for process: %s\n", strerror(errno));
            return false;
        }

        if (WIFEXITED(wstatus)) {
            int estatus = WEXITSTATUS(wstatus);
            if (estatus) {
                fprintf(stderr, "Process exited with status %d\n", estatus);
                return false;
            }

            return true;
        }

        if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "Process was terminated by signal %d (%s)\n", WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
            return false;
        }
    }
    assert(0);
}

int process_start(const char* args[], size_t args_count) {
    char** args_copy = PROCESS_MALLOC((args_count + 1) * sizeof(char*));
    PROCESS_ASSERT(args_copy != NULL);

    memcpy(args_copy, args, args_count * sizeof(char*));
    args_copy[args_count] = NULL;

    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        PROCESS_FREE(args_copy);

        return -1;
    } else if (pid == 0) {
        execvp(args_copy[0], args_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", args_copy[0], strerror(errno));
        exit(1);
    }

    PROCESS_FREE(args_copy);
    return pid;
}

bool process_run(const char* args[], size_t args_count) {
    int pid = process_start(args, args_count);
    return process_wait_for_exit(pid);
}

Proc process_start_ex(const char* args[], size_t args_count, uint8_t pipe_flags) {
    Proc proc;
    proc.pid = -1;

    char** args_copy = PROCESS_MALLOC((args_count + 1) * sizeof(char*));
    PROCESS_ASSERT(args_copy != NULL);

    memcpy(args_copy, args, args_count * sizeof(char*));
    args_copy[args_count] = NULL;

    bool should_pipe_in = (pipe_flags & PROCESS_PIPE_STDIN) != 0;
    bool should_pipe_out = (pipe_flags & PROCESS_PIPE_STDOUT) != 0;
    bool should_pipe_errout = (pipe_flags & PROCESS_PIPE_STDERR) != 0;

    if (should_pipe_in) {
        if (!process_pipe(proc.in_pipe)) return proc;
    }

    if (should_pipe_out || should_pipe_errout) {
        if (!process_pipe(proc.out_pipe)) return proc;
    }

    proc.pid = fork();
    if (proc.pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return proc;
    } else if (proc.pid == 0) {
        if (should_pipe_in) {
            if (!process_dup2(proc.in_pipe[PIPE_READ], STDIN_FILENO)) exit(1);
            if (!process_close(proc.in_pipe[PIPE_WRITE])) exit(1);
        }

        if (should_pipe_out) {
            if (!process_dup2(proc.out_pipe[PIPE_WRITE], STDOUT_FILENO)) exit(1);
        }

        if (should_pipe_errout) {
            if (!process_dup2(proc.out_pipe[PIPE_WRITE], STDERR_FILENO)) exit(1);
        }

        if (should_pipe_out || should_pipe_errout) {
            if (!process_close(proc.out_pipe[PIPE_READ])) exit(1);
        }

        execvp(args_copy[0], args_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", args_copy[0], strerror(errno));
        exit(1);
    }

    if (should_pipe_in) {
        if (!process_close(proc.in_pipe[PIPE_READ])) exit(1);
    }

    if (should_pipe_out || should_pipe_errout) {
        if (!process_close(proc.out_pipe[PIPE_WRITE])) exit(1);
    }

    PROCESS_FREE(args_copy);
    return proc;
}

bool process_proc_destroy(Proc* proc) {
    if (!process_close(proc->in_pipe[PIPE_WRITE])) return false;
    if (!process_close(proc->out_pipe[PIPE_READ])) return false;
    return true;
}

#endif // PROCESS_IMPLEMENTATION
#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    char* items;
    size_t count;
    size_t capacity;
}StringBuilder;
typedef StringBuilder SB;

// Resizes a string builder 
void sb_maybe_resize(StringBuilder* sb, size_t to_append_len);

// Pushes a character to a string builder 
void sb_push(StringBuilder* sb, char c);

#define sb_push_null(sb) sb_push(sb, 0)

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

// Writes the contents of a string builder to a file descriptor
#define sb_write(sb, fd) write(fd, sb.items, sb.count)
// Writes the contents of a string builder to a file handle
#define sb_fwrite(sb, f) fwrite((sb)->items, 1, (sb)->count, f)
// Writes the contents of a string builder to a file
bool sb_write_file(StringBuilder* self, const char* filepath);

bool sb_read(StringBuilder* sb, int fd, size_t n);
bool sb_fread(StringBuilder* sb, FILE* f, size_t n);

// Reads the provided file and appends its contents into the provided SB.
bool sb_read_file(StringBuilder* sb, const char* filepath); 

#ifndef SB_NO_CURL
// Downloads file with CURL
bool sb_download_file(StringBuilder* sb, const char* url);
#endif // SB_NO_CURL

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
#undef SB_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef SB_NO_CURL
#include <curl/curl.h>
#endif // SB_NO_CURL
#include <errno.h>
#include <unistd.h>

#ifndef SB_MALLOC
    #define SB_MALLOC malloc
#endif // SB_MALLOC

#ifndef SB_INIT_CAP
#define SB_INIT_CAP 8
#endif // SB_INIT_CAP

size_t __curl_sb(void *contents, size_t sz, size_t nmemb, void *ctx) {
    size_t realsize = sz * nmemb;

    sb_maybe_resize(ctx, realsize);
    sb_push_nstr(ctx, contents, realsize);

    return realsize;
}

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

#ifndef SB_NO_CURL
bool sb_download_file(StringBuilder* sb, const char* url) {
    CURL* h = curl_easy_init();
    if (h == NULL) {
        fprintf(stderr, "Couldn't init CURL context\n");
        return false;
    }

    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, __curl_sb);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, sb);

    CURLcode ret = curl_easy_perform(h);
    if (ret != CURLE_OK) {
        fprintf(stderr, "Couldn't download %s: %s\n", url, curl_easy_strerror(ret));
        curl_easy_cleanup(h);
        return false;
    }

    curl_easy_cleanup(h);
    return true;
}
#endif // SB_NO_CURL

bool sb_read(StringBuilder* sb, int fd, size_t n) {
    sb_maybe_resize(sb, n);
    read(fd, sb->items + sb->count, n);
    sb->count += n;
    return true;
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
bool sv_ends_with(StringView self, StringView suffix);
bool sv_starts_with(StringView self, StringView prefix);

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

bool sv_cmpsv(StringView sv, StringView that) {
    if (sv.len != that.len) return false;

    for (size_t i = 0; i < sv.len; ++i) {
        if (sv.start[i] != that.start[i]) return false;
    }

    return true;
}

bool sv_ends_with(StringView self, StringView suffix) {
    if (self.len < suffix.len) return false;

    return strncmp(self.start + self.len - suffix.len, suffix.start, suffix.len) == 0;
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
#ifndef TSPRINTF_H_
#define TSPRINTF_H_
#include <stddef.h>

__attribute__((format(printf, 1, 2)))
char* tsprintf(const char* fmt, ...);
size_t tsnapshot();
void trewind(size_t snapshot);
void treset();

#endif // TSPRINTF_H_

#ifdef TSPRINTF_IMPLEMENTATION
#undef TSPRINTF_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>

#ifndef TBUFFER_SIZE
#define TBUFFER_SIZE (1 << 10)
#endif // TBUFFER_SIZE

static char tbuffer[TBUFFER_SIZE] = {0};
static size_t tbuffer_size = 0;

char* tsprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (n + tbuffer_size >= TBUFFER_SIZE) {
        return NULL;
    }

    char* start = tbuffer + tbuffer_size;

    va_start(args, fmt);
    int n_ = vsnprintf(start, n + 1, fmt, args);
    assert(n == n_);

    tbuffer_size += n_;
    va_end(args);

    return start;
}

size_t tsnapshot() {
    return tbuffer_size;
}

void trewind(size_t snapshot) {
    tbuffer_size = snapshot;
}

void treset() {
    tbuffer_size = 0;
}

#endif // TSPRINTF_IMPLEMENTATION
#ifndef TYPES_H_
#define TYPES_H_
#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned __int128 u128;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef __int128 i128;

typedef float f32;
typedef double f64;

typedef char* cstr;

typedef FILE* Fp;
typedef void* rawp;

#endif // TYPES_H_

#ifndef UTILS_H_
#define UTILS_H_
#include <stdbool.h>

#ifndef MACROS_ABORT
#include <stdlib.h>
#define MACROS_ABORT abort
#endif // MACROS_ABORT

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define PANIC(...) do { eprintf(__VA_ARGS__); MACROS_ABORT(); } while (0)

#define TODO() PANIC("%s:%d: TODO: %s not implemented yet\n", __FILE__, __LINE__, __func__)
#define TODOO(thing) PANIC("%s:%d: TODO: %s not implemented yet\n", __FILE__, __LINE__, #thing)

#define UNIMPLEMENTED() PANIC("%s:%d: UNIMPLEMENTED: %s not implemented\n", __FILE__, __LINE__, __func__)
#define UUNIMPLEMENTED(thing) PANIC("%s:%d: UNIMPLEMENTED: %s not implemented\n", __FILE__, __LINE__, #thing)

#define UNREACHABLE() PANIC("%s:%d: UNREACHABLE\n", __FILE__, __LINE__)

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

#define return_defer(thing) do { result = thing; goto defer; } while (0)
#define gcc_format_attr(last_pos, start_va) __attribute__((format(printf, last_pos, start_va)))

#include <signal.h>
#define BREAKPOINT() do { eprintf("%s:%d: BREAKPOINT", __FILE__, __LINE__); raise(SIGINT); } while (0)
#define BREAKPOINT_IF() do { if (expr) { eprintf("%s:%d: BREAKPOINT", __FILE__, __LINE__); raise(SIGINT); } } while (0)

bool dynlib_load(void** handle, const char* path, int mode);
bool dynsym_load(void** sym, void* handle, const char* name);

#define simple_log_info(...) do { printf("[INFO] "); printf(__VA_ARGS__); } while (0)
#define simple_log_warn(...) do { printf("[WARNING] "); printf(__VA_ARGS__); } while (0)
#define simple_log_err(...) do { printf("[ERROR] "); printf(__VA_ARGS__); } while (0)
#define simple_flog_info(f, ...) do { fprintf(f, "[INFO] "); fprintf(f, __VA_ARGS__); } while (0)
#define simple_flog_warn(f, ...) do { fprintf(f, "[WARNING] "); fprintf(f, __VA_ARGS__); } while (0)
#define simple_flog_err(f, ...) do { fprintf(f, "[ERROR] "); fprintf(f, __VA_ARGS__); } while (0)

#endif // UTILS_H_

#ifdef UTILS_IMPLEMENTATION
#undef UTILS_IMPLEMENTATION

#include <dlfcn.h>
#include <stdio.h>

bool dynlib_load(void** handle, const char* path, int mode) {
    if (*handle != NULL)
        dlclose(*handle);

    *handle = dlopen(path, mode);
    if (*handle == NULL) {
        fprintf(stderr, "%s: %s\n", __func__, dlerror());
        return false;
    }

    return true;
}

bool dynsym_load(void** sym, void* handle, const char* name) {
    *sym = dlsym(handle, name);
    if (*sym == NULL) {
        fprintf(stderr, "%s: %s\n", __func__, dlerror());
        return false;
    }

    return true;
}

#endif // UTILS_IMPLEMENTATION
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

