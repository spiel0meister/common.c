#ifndef LOGGER_H_
#define LOGGER_H_
#include <stdio.h>

typedef enum LogLevel {
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,

	LOG_COUNT_
}LogLevel;

typedef struct Logger {
	FILE* f;
	LogLevel level;
}Logger;

void log_log(Logger* self, LogLevel level, const char* fmt, ...);
#define log_info(logger, ...) log_log(logger, LOG_INFO, __VA_ARGS__)
#define log_warn(logger, ...) log_log(logger, LOG_WARN, __VA_ARGS__)
#define log_error(logger, ...) log_log(logger, LOG_ERROR, __VA_ARGS__)

extern Logger global_logger;
void log_init_global(FILE* f, LogLevel level);
#define log_log_global(level, ...) log_log(&global_logger, level, __VA_ARGS__)
#define log_info_global(...) log_log(&global_logger, LOG_INFO, __VA_ARGS__)
#define log_warn_global(...) log_log(&global_logger, LOG_WARN, __VA_ARGS__)
#define log_error_global(...) log_log(&global_logger, LOG_ERROR, __VA_ARGS__)

#endif // LOGGER_H_

#ifdef LOGGER_IMPLEMENTATION
#undef LOGGER_IMPLEMENTATION
#include <stdarg.h>

#define log__static_assert _Static_assert

log__static_assert(LOG_COUNT_ == 3, "Log level was added");
static const char* loglevel_to_str[LOG_COUNT_] = {
	[LOG_INFO] = "INFO",
	[LOG_WARN] = "WARN",
	[LOG_ERROR] = "ERROR",
};
static const char* loglevel_to_color[LOG_COUNT_] = {
	[LOG_INFO] = "\x1B[34m",
	[LOG_WARN] = "\x1B[33m",
	[LOG_ERROR] = "\x1B[31m",
};

Logger global_logger;

void log_init_global(FILE* f, LogLevel level) {
	global_logger.f = f;
	global_logger.level = level;
}

__attribute__((format(printf, 3, 4)))
void log_log(Logger* self, LogLevel level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	if (self->level <= level) {
		fprintf(self->f, "[%s%s\x1B[0m] ", loglevel_to_color[level], loglevel_to_str[level]);
		vfprintf(self->f, fmt, args);
		fprintf(self->f, "\n");
	}

	va_end(args);
}

#endif // LOGGER_IMPLEMENTATION
