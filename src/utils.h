#ifndef UTILS_H
#define UTILS_H

#define loop while (true)

char* argv_pop(int* argc, char*** argv);

char* read_entire_file(char* filepath);

#endif // UTILS_H

#ifdef UTILS_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char* argv_pop(int* argc, char*** argv) {
    assert(argc > 0);
    (*argc)++;
    return *(*argv)++;
}

#ifndef FILE_CONTENT_MALLOC 
#define FILE_CONTENT_MALLOC malloc
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
