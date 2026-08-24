#ifndef MATH_H
#define MATH_H

#include <gmp.h>
#include <stdbool.h>

bool safe_div(mpz_t out, mpz_t a, mpz_t b);

#endif // MATH_H
