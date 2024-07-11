#ifndef LOG_H_
#define LOG_H_
#include <stdio.h>

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
void log_set_file(FILE* f);
void log_set_level(LogLevel level);

#endif // LOG_H_

#ifdef LOG_IMPLEMENTATION
#include <stdarg.h>

typedef struct {
    LogLevel level;
    FILE* file;
}Logger;

static_assert(LOG_COUNT_ == 3, "LogLevel enum grew");
static char* LEVEL_TO_STR[] = {
    [LOG_ERROR] = "ERROR",
    [LOG_WARN] = "WARN",
    [LOG_INFO] = "INFO",
};

static Logger logger = { LOG_INFO, NULL };
void log_log(LogLevel level, const char* fmt, ...) {
    if (logger.file == NULL) {
        logger.file = stdout;
        log_info("log file not set, default to stdout\n");
    }

    va_list args;
    va_start(args, fmt);
    if (level >= logger.level) {
        fprintf(logger.file, "[%s] ", LEVEL_TO_STR[level]);
        vfprintf(logger.file, fmt, args);
    }
    va_end(args);
}

void log_set_file(FILE* f) {
    logger.file = f;
}

void log_set_level(LogLevel level) {
    logger.level = level;
}
#endif

