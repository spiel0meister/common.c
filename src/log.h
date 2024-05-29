#ifndef LOG_H
#define LOG_H

typedef enum {
    LOG_INFO = 0,
    LOG_WARN,
    LOG_ERROR,

    LOG_COUNT_
}LogLevel;

#define log_info(...) log_log(LOG_INFO, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __VA_ARGS__)

__attribute__((format(printf, 2, 3)))
void log_log(LogLevel level, const char* fmt, ...);

void log_set_level(LogLevel level);

#endif // LOG_H

#ifdef LOG_IMPLEMENTATION
#include <stdarg.h>
#include <stdio.h>

static char* LEVEL_TO_STR[] = {
    [LOG_ERROR] = "ERROR",
    [LOG_WARN] = "WARN",
    [LOG_INFO] = "INFO",
};
static_assert(LOG_COUNT_ == 3, "LogLevel enum grew");

static LogLevel log_level = LOG_INFO;
void log_log(LogLevel level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (level >= log_level) {
        printf("[%s] ", LEVEL_TO_STR[level]);
        vprintf(fmt, args);
        printf("\n");
    }
    va_end(args);
}

void log_set_level(LogLevel level) {
    log_level = level;
}
#endif

