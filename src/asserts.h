#ifndef ASSERTS_H
#define ASSERTS_H

#define panic(...) fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE)

#define todo() fprintf(stderr, "%s not yet implemented", __func__); exit(EXIT_FAILURE)
#define todo_msg(...) fprintf(stderr, "%s not yet implemented: ", __func__); fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE)

#define unimplemented() fprintf(stderr, "%s not implemented", __func__); exit(EXIT_FAILURE)
#define unimplemented_msg(...) fprintf(stderr, "%s not implemented: ", __func__); fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE)

#define assert_eq(lhs, rhs) assert(lhs == rhs)
#define assert_neq(lhs, rhs) assert(lhs != rhs)

#define dbg(expr, fmt) \
    printf("%s = ", #expr); \
    printf(fmt, expr); \
    printf("\n")

#endif // ASSERTS_H
