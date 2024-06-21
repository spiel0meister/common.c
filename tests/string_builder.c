#define TEST_IMPLEMENTATION
#include "../ctest.h"

#define SB_IMPLEMENTATION
#include "../src/string_builder.h"

Test(sb_push_test) {
    StringBuilder sb = {0};
    sb_push_str(&sb, "hello");
    test_assert(sb.items != NULL);
    test_assert(sb.count == 5);

    sb_push_str(&sb, "h", "n");
    test_assert(sb.items != NULL);
    test_assert(sb.count == 7);
    sb_free(&sb);

    return TEST_SUCCES;
}

Test(sb_export_test) {
    StringBuilder sb = {0};
    sb_push_str(&sb, "hello");

    char* out = sb_export(&sb);
    test_assert_str_eq(out, "hello");
    free(out);
    
    sb_push_str(&sb, " world");
    out = sb_export(&sb);

    test_assert_str_eq(out, "hello world");
    free(out);

    return TEST_SUCCES;
}

Test(sb_export_inplace_test) {
    StringBuilder sb = {0};
    sb_push_str(&sb, "hello");
    
    char out[16] = {};
    sb_export_inplace(&sb, out);
    test_assert_str_eq(out, "hello");

    sb_push_str(&sb, " world");
    sb_export_inplace(&sb, out);
    test_assert_str_eq(out, "hello world");

    sb_free(&sb);

    return TEST_SUCCES;
}

int main(void) {
    test_register(sb_push_test);
    test_register(sb_export_test);
    test_register(sb_export_inplace_test);

    test_run_all_sync(true);
    return 0;
}
