#define TEST_IMPLEMENTATION
#include "../ctest.h"

#define ARENA_IMPLEMENTATION
#include "../src/arena.h"

Test(arena_prealloc_test) {
    Arena arena = {};
    arena_prealloc(&arena, sizeof(int) * 1);
    test_assert(arena_alloced(&arena));

    arena_free(&arena);

    return TEST_SUCCES;
}

Test(arena_alloc_test) {
    Arena arena = {};
    arena_prealloc(&arena, sizeof(int) * 1);

    int* a = arena_alloc(&arena, int);
    test_assert(a != NULL);

    int* b = arena_alloc(&arena, int);
    test_assert(b == NULL);

    arena_free(&arena);

    return TEST_SUCCES;
}

int main(void) {
    test_register(arena_prealloc_test);
    test_register(arena_alloc_test);
    test_run_all_sync(true);
    return 0;
}
