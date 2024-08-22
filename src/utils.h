#ifndef UTILS_H_
#define UTILS_H_
#include <stdbool.h>

#include <dlfcn.h>

bool dynlib_load(void** handle, const char* path, int mode);
bool dynsym_load(void** sym, void* handle, const char* name);

#endif // UTILS_H_

#ifdef UTILS_IMPLEMENTATION
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
