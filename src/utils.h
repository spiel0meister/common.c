#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

char* pop_argv(int* argc, char*** argv);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UTILS_H_

#ifdef UTILS_IMPLEMENTATION
char* pop_argv(int* argc, char*** argv) {
    char* arg = **argv;
    (*argc)--;
    (*argv)++;
    return arg;
}
#endif
