#ifndef UTILS_H_
#define UTILS_H_

char* pop_argv(int* argc, char*** argv);

#endif // UTILS_H_

#ifdef UTILS_IMPLEMENTATION
char* pop_argv(int* argc, char*** argv) {
    char* arg = **argv;
    (*argc)--;
    (*argv)++;
    return arg;
}
#endif
