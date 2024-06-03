#define TEST_IMPLEMENTATION
#include "../ctest.h"

#define RANDOM_IMPLEMENTATION
#include "../src/random.h"

Test(randint_test) {
    test_assert_in_range(1, 100, randint(1, 100));
    test_assert_in_range(1, 10, randint(1, 10));

    return TEST_SUCCES;
}

Test(random_test) {
    float f = random();
    test_assert_in_range(0.0f, 1.0f, f);

    return TEST_SUCCES;
}

int main(void) {
    srand(time(NULL));
    test_register(randint_test);
    test_register(random_test);

    test_run_all_sync(true);
    return 0;
}
