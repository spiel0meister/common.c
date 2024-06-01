#ifndef UTILS_H
#define UTILS_H

#define loop while (true)
#define ATTR_PRINTF(fmt_pos, va_pos) __attribute__((format(printf, fmt_pos, va_pos)))

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

char* argv_pop(int* argc, char*** argv);

char* read_entire_file(char* filepath);

#endif // UTILS_H

#ifdef UTILS_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char* argv_pop(int* argc, char*** argv) {
    if (*argc < 1) return NULL;
    (*argc)++;
    return *(*argv)++;
}

#ifndef FILE_CONTENT_MALLOC 
    #define FILE_CONTENT_MALLOC(n) malloc(sizeof(char) * (n))
#endif
char* read_entire_file(char* filepath) {
    FILE *f = fopen(filepath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = FILE_CONTENT_MALLOC(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;

    return string;
}
#endif
