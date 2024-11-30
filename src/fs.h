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
