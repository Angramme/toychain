#include "rsa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/overflow.h"

/**
 * @file rsa.c
 * @brief a collection of RSA related functions
 * 
 */

/**
 * @brief generates public and private RSA keys
 * public = (s, n) ; private = (u, n)
 *
 * @param p prime
 * @param q prime
 * @param n return value
 * @param s return value
 * @param u return value
 * 
 * @return true : the generation was successfull!
 * @return false : is most probably impossible to generate a valid key pair! 
 */
bool generate_key_values(int64 p, int64 q, int64* n, int64* s, int64* u) {
    *n = -1;
    *s = -1;
    *u = -1;
    if(p < 0 || q < 0) return false;
    
    int64 N = p * q;
    
    // overflow protection!
    // if this overflows then modulo multiplication of any numbers might overflow as well!
    if(MULTIPLICATION_OVERFLOW_CHECK_i64(N, N)) return false; 

    int64 T = (p - 1) * (q - 1);
    // avoid infinite loop
    for (int I = 0; I < (1 << 16); I++) {
        // rand() should probably be replaced with 
        // a truly random, random function
        int64 S = rand_int64(3, T-1);
        int64 U, V;
        int64 pgcd = extended_gcd(S, T, &U, &V);
        if (pgcd != 1 || U < 0) continue;
        *n = N;
        *s = S;
        *u = U;
        return true;
    }
    return false;
}

int64* encrypt(char* chaine, int64 s, int64 n) {
    int N = strlen(chaine);
    int64* ret = malloc(sizeof(int64) * (N + 1));
    ret[N] = '\0';
    for (int i = 0; i < N; i++) {
        int m = chaine[i];
        ret[i] = modpow(m, s, n);
    }
    return ret;
}

char* decrypt(int64* crypted, int size, int64 u, int64 n) {
    char* ret = (char*)malloc(sizeof(char)*(size+1));
    ret[size] = '\0';
    for(int i=0; i<size; i++){        
        int64 c = crypted[i];
        ret[i] = modpow(c,u,n);
    }
    return ret;
}
