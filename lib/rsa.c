#include "rsa.h"

/**
 * @brief generates public and private RSA keys
 * public = (s, n) ; private = (u, n)
 * 
 * @param p prime
 * @param q prime
 * @param n return value
 * @param s return value
 * @param u return value
 */
void generate_key_values(long p, long q, long* n, long* s, long* u){
    long N = p*q;
    long T = (p-1)*(q-1);
    // avoid infinite loop
    for(int I=0; I<(1<<16); I++){
        // rand() should probably be replaced with 
        // a truly random, random function
        long S = rand() % T;
        long U, V;
        long pgcd = extended_gcd(S, T, &U, &V);
        if(pgcd != 1) continue;
        *n = N;
        *s = S;
        *u = U;
        return;
    }
    *n = -1;
    *s = -1;
    *u = -1;
}