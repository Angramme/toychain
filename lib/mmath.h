#ifndef __MMATH_H
#define __MMATH_H

#include <stdbool.h>
#include "lib/types.h"

#define GOLDEN_RATIO 0.61803398875

int64 rand_int64(int64 low, int64 up);

bool is_prime_naive(int64 p);
bool is_prime_miller(int64 p, int k);
int64 random_prime_number(int low_size, int up_size, int k);

int64 modpow_naive(int64 a, int64 m, int64 n);
int64 modpow(int64 a, int64 m, int64 n);
int64 modpow_r(int64 a, int64 m, int64 n);

int64 extended_gcd(int64 s, int64 t, int64* u, int64* v);

uint32 jenkins_one_at_a_time_hash(const uint8* key, size_t len);

#endif