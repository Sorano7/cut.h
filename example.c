#define CUT_IMPLEMENTATION
#include "cut.h"

int divide(int a, int b)
{
    DEV_MUST(b != 0);
    DEV_DEBUG("dividing %d by %d", a, b);
    return a / b;
}

TEST(ten_div_two_equals_five)
{
    CHECK(divide(10, 2) == 5);
}

TEST_RUN()
