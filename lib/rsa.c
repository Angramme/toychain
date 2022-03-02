#include "rsa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
 */
void generate_key_values(long p, long q, long* n, long* s, long* u) {
    long N = p * q;
    long T = (p - 1) * (q - 1);
    // avoid infinite loop
    for (int I = 0; I < (1 << 16); I++) {
        // rand() should probably be replaced with 
        // a truly random, random function
        long S = rand_long(3, T-1);
        long U, V;
        long pgcd = extended_gcd(S, T, &U, &V);
        if (pgcd != 1) continue;
        *n = N;
        *s = S;
        *u = U;
        return;
    }
    fprintf(stderr, "ERROR: coouldn't generate the private public key pair!!!\n");
    *n = -1;
    *s = -1;
    *u = -1;
}

long* encrypt(char* chaine, long s, long n) {
    int N = strlen(chaine);
    long* ret = malloc(sizeof(long) * (N + 1));
    for (int i = 0; i < N; i++) {
        int m = chaine[i];
        ret[i] = modpow(m, s, n);
    }
    ret[N] = '\0';
    return ret;
}

char* decrypt(long* crypted, int size, long u, long n) {
    char* ret = (char*)malloc(sizeof(char)*(size+1));
    ret[size] = '\0';
    for(int i=0; i<size; i++){
        
        
        long c = crypted[i];
        long ldc = modpow(c,u,n);
        ret[i] = ldc;
        

        //Create a segfault
        /*
        long c = crypted[i];
        ret[i] = modpow(c, u, n);
        */
    }
    return ret;
}
