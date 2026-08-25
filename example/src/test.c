#define CUT_IMPL
#include "../../cut.h"

#include "my_math.h"

#define FIXTURE_START(va, vb, vc) \
    mpz_t a, b, c; \
    mpz_inits(a, b, c, NULL); \
    mpz_set_ui(a, va); \
    mpz_set_ui(b, vb); \
    mpz_set_ui(c, vc);

#define FIXTURE_END() \
    mpz_clears(a, b, c, NULL);

#define CHECK_EQ(z, n) CUT_CHECK(mpz_cmp_ui((z), (n)) == 0)

TEST(integer_division_works)
{
    FIXTURE_START(100, 50, 0);
        CUT_MUST(safe_div(c, a, b));
        CHECK_EQ(c, 2);
    FIXTURE_END()
}

TEST(division_by_zero_fails)
{
    FIXTURE_START(100, 0, 0);
        bool result = safe_div(c, a, b);
        CUT_MUST(result == false);
    FIXTURE_END()
}

TEST_RUN()
