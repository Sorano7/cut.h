#define CUT_IMPLEMENTATION
#include "cut.h"

TEST(first_test)
{
    MUST(1 + 1 == 2);
}

TEST(second_test)
{
    DEBUGF("This is a debug log.");
    CHECK("CHECK may fail." && false);
    MUST("MUST must pass" && false);
}

TEST_RUN()
