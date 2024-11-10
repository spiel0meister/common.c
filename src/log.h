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
