#ifndef MACROS_H_
#define MACROS_H_

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

#endif // MACROS_H_
