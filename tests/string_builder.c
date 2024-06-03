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

Test(sbuilder_push_test) {
    StringBuilder sb = sbuilder_init(10);
    sbuilder_push_str(&sb, "hello");
    test_assert(sb.items != NULL);
    test_assert(sb.capacity == 10);
    test_assert(sb.size == 5);

    sbuilder_push_str(&sb, "h", "n");
    test_assert(sb.items != NULL);
    test_assert(sb.capacity == 10);
    test_assert(sb.size == 7);
    sbuilder_free(&sb);

    return TEST_SUCCES;
}

Test(sbuilder_export_test) {
    StringBuilder sb = sbuilder_init(10);
    sbuilder_push_str(&sb, "hello");

    char* out = sbuilder_export(&sb);
    test_assert_str_eq(out, "hello");
    free(out);
    
    sbuilder_push_str(&sb, " world");
    out = sbuilder_export(&sb);

    test_assert_str_eq(out, "hello world");
    free(out);

    return TEST_SUCCES;
}

Test(sbuilder_export_inplace_test) {
    StringBuilder sb = sbuilder_init(10);
    sbuilder_push_str(&sb, "hello");
    
    char out[16] = {};
    sbuilder_export_inplace(&sb, out);
    test_assert_str_eq(out, "hello");

    sbuilder_push_str(&sb, " world");
    sbuilder_export_inplace(&sb, out);
    test_assert_str_eq(out, "hello world");

    sbuilder_free(&sb);

    return TEST_SUCCES;
}

int main(void) {
    test_register(sbuilder_init_test);
    test_register(sbuilder_push_test);
    test_register(sbuilder_export_test);
    test_register(sbuilder_export_inplace_test);

    test_run_all_sync(true);
    return 0;
}
