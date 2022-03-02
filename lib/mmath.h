#ifndef __MMATH_H
#define __MMATH_H

#include <stdbool.h>
#include "types.h"

int64 rand_int64(int64 low, int64 up);

bool is_prime_naive(int64 p);
bool is_prime_miller(int64 p, int k);
int64 random_prime_number(int low_size, int up_size, int k);

int64 modpow_naive(int64 a, int64 m, int64 n);
int64 modpow(int64 a, int64 m, int64 n);
int64 modpow_r(int64 a, int64 m, int64 n);

int64 extended_gcd(int64 s, int64 t, int64* u, int64* v);

#endif