#define TEST_IMPLEMENTATION
#include "../ctest.h"

#define SBUILDER_IMPLEMENTATION
#include "../src/string_builder.h"

Test(sbuilder_init_test) {
    StringBuilder sb = sbuilder_init(2);
    test_assert(sb.capacity == 2);
    test_assert(sb.items != NULL);

    sbuilder_free(&sb);

    return TEST_SUCCES;
}

int main(void) {
    test_register(sbuilder_init_test);

    test_run_all_sync(true);
    return 0;
}
