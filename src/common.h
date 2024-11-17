#ifndef COMMON_H_
#define COMMON_H_


#ifdef COMMON_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#define CPERF_IMPLEMENTATION
#ifndef COMMON_USE_DA
#define DAH_IMPLEMENTATION
#endif // COMMON_USE_DA
#define EASINGS_IMPLEMENTATION
#define FLAG_IMPLEMENTATION
#define FS_IMPLEMENTATION
#define LC_IMPLEMENTATION
#define LINEAR_IMPLEMENTATION
#define LOG_IMPLEMENTATION
#ifdef COMMON_USE_MSTR
#define MODERN_STRING_IMPLEMENTATION
#else // COMMON_USE_MSTR
#define STRING_BUILDER_IMPLEMENTATION
#define STRING_VIEW_IMPLEMENTATION
#endif // COMMON_USE_MSTR
#define SUBPROCCES_IMPLEMENTATION
#define TSPRINTF_IMPLEMENTATION
#define UTILS_IMPLEMENTATION
#endif

#include "./arena.h"
#include "./cperf.h"
#include "./da.h"
#include "./dah.h"
#include "./easings.h"
#include "./flag.h"
#include "./fs.h"
#include "./leakcheck.h"
#include "./linear.h"
#include "./log.h"
#include "./mstr.h"
#include "./string_builder.h"
#include "./string_view.h"
#include "./subprocces.h"
#include "./tsprintf.h"
#include "./types.h"
#include "./utils.h"

#endif // COMMON_H_
