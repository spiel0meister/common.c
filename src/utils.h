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
