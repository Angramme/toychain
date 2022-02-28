#ifndef _PRIME_H
#define _PRIME_H

#include <stdbool.h>

bool is_prime_naive(long p);
bool is_prime_miller(long p, int k);
long random_prime_number(int low_size, int up_size, int k);

#endif