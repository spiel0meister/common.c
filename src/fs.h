#ifndef FS_H_
#define FS_H_
#endif // FS_H_

#include <stddef.h>

char* read_file(const char* path, size_t* size);

#ifdef FS_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char* read_file(const char* path, size_t* size) {
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", path, strerror(errno));
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size_ = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* s = malloc(size_ + 1);
    fread(s, 1, size_, f);
    fclose(f);

    if (size != NULL) *size = size_;
    return s;
}

#endif // FS_IMPLEMENTATION
