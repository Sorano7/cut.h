#include <stdio.h>
#include <gmp.h>
#include "my_math.h"

int main(void)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, NULL);

    mpz_set_ui(a, 10);
    mpz_set_ui(b, 5);

    if (!safe_div(c, a, b))
        printf("Division by zero\n");
    else
        gmp_printf("%Zd\n", c);

    mpz_clears(a, b, c, NULL);
    return 0;
}
