#define TEST_IMPLEMENTATION
#include "../ctest.h"

#define BSTRING_IMPLEMENTATION
#include "../src/bstring.h"

Test(shortf_test) {
    ShortString a = shortf("%d %d", 1, 2);
    test_assert_str_eq(a.buf, "1 2");

    a = shortf("%s %d", a.buf, 3);
    test_assert_str_eq(a.buf, "1 2 3");

    a = shortf("%s %.1f", a.buf, 2.5f);
    test_assert_str_eq(a.buf, "1 2 3 2.5");

    return TEST_SUCCES;
}

Test(str_start_endswith_test) {
    test_assert(str_startswith("hello", "hell"));
    test_assert(str_startswith("world", "wor"));
    test_assert_not(str_startswith("google", "lol"));
    

    test_assert(str_endswith("a.txt", ".txt"));
    test_assert(str_endswith("a.png", ".png"));
    test_assert_not(str_endswith("google", "lol"));

    return TEST_SUCCES;
}

int main(void) {
    test_register(shortf_test);
    test_register(str_start_endswith_test);
    test_run_all_sync(true);
    return 0;
}
