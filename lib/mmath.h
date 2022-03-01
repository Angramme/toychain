#ifndef __MMATH_H
#define __MMATH_H

#include <stdbool.h>

long rand_long(long low, long up);

bool is_prime_naive(long p);
bool is_prime_miller(long p, int k);
long random_prime_number(int low_size, int up_size, int k);

long modpow_naive(long a, long m, long n);
long modpow(long a, long m, long n);
long modpow_r(long a, long m, long n);

long extended_gcd(long s, long t, long* u, long* v);

#endif