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

            for (size_t i = 0; i < flags_size; ++i) {
                if (strcmp(flags[i].name, arg) == 0) {
                    switch (flags[i].type) {
                    case FLAG_OPTION:
                        flags[i].as.option = true;
                        break;
                    case FLAG_STR:
                        if (argc < 1) {
                            fprintf(stderr, "Missing argument for flag -%s\n", arg);
                            return -1;
                        }

                        flags[i].as.cstr = flag__shift_args(&argc, &argv);
                        break;
                    case FLAG_INT:
                        if (argc < 1) {
                            fprintf(stderr, "Missing argument for flag -%s\n", arg);
                            return -1;
                        }

                        flags[i].as.i = atoi(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_FLOAT:
                        if (argc < 1) {
                            fprintf(stderr, "Missing argument for flag -%s\n", arg);
                            return -1;
                        }

                        flags[i].as.f = atof(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_LONG:
                        if (argc < 1) {
                            fprintf(stderr, "Missing argument for flag -%s\n", arg);
                            return -1;
                        }

                        flags[i].as.l = atol(flag__shift_args(&argc, &argv));
                        break;
                    case FLAG_LLONG:
                        if (argc < 1) {
                            fprintf(stderr, "Missing argument for flag -%s\n", arg);
                            return -1;
                        }

                        flags[i].as.ll = atoll(flag__shift_args(&argc, &argv));
                        break;
                    default:
                        assert(0 && "unreachable");
                        break;
                    }
                }
            }
        } else {
            return saved_argc - argc;
        }
    }

    return saved_argc - argc;
}


#endif
