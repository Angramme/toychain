#ifndef __RSA_H
#define __RSA_H

#include "lib/mmath.h"
#include "lib/types.h"
#include <stdbool.h>

bool generate_key_values(int64 p, int64 q, int64* n, int64* s, int64* u);
int64* encrypt(char* chaine, int64 s, int64 n);
char* decrypt(int64* crypted, int size, int64 u, int64 n);

#endif