#ifndef LOG_H
#define LOG_H
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

#endif // LOG_H

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
        printf("[%s] ", LEVEL_TO_STR[LOG_INFO]);
        printf("log file not set, default to stdout");
        printf("\n");
        logger.file = stdout;
    }

    va_list args;
    va_start(args, fmt);
    if (logger.file == stdout) {
        fprintf(logger.file,"[%s] ", LEVEL_TO_STR[level]);
        vfprintf(logger.file, fmt, args);
        fprintf(logger.file, "\n");
    } else if (level >= logger.level) {
        fprintf(logger.file,"[%s] ", LEVEL_TO_STR[level]);
        vfprintf(logger.file, fmt, args);
        fprintf(logger.file, "\n");
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

