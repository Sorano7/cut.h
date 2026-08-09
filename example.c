#define CUT_IMPLEMENTATION
#include "cut.h"

int divide(int a, int b)
{
    DEBUGF("b = %d", b);
    return a + b;
}

TEST(one_plus_one_equals_two)
{
    MUST(1 + 1 == 2);
}

TEST(two_divide_by_one_equals_two)
{
    MUST(divide(2, 1));
}

TEST_RUN()
