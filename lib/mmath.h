#ifndef __MMATH_H
#define __MMATH_H

#include <stdbool.h>

bool is_prime_naive(long p);
bool is_prime_miller(long p, int k);
long random_prime_number(int low_size, int up_size, int k);

long modpow_naive(long a, long m, long n);
int modpow(long a, long m, long n);
int modpow_r(long a, long m, long n);

#endif