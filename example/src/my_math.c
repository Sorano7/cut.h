#include "my_math.h"

bool safe_div(mpz_t out, mpz_t a, mpz_t b)
{
    if (mpz_cmp_ui(b, 0) == 0)
        return false;
    mpz_div(out, a, b);
    return true;
}
