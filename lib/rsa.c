#include "lib/rsa.h"
#include "lib/error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "lib/overflow.h"

/**
 * @file rsa.c
 * @brief a collection of RSA related functions
 * 
 */

/**
 * @brief initializes a key struct
 *
 * @param p prime
 * @param q prime
 * @return void
 */
void init_key(Key* key, int64 val, int64 n){
    assert(key);
    key->v = val;
    key->n = n;
}

/**
 * @brief copies key
 * 
 * @param o 
 * @return Key* 
 */
Key* copy_key(const Key* o){
    Key* ret = malloc(sizeof(Key));
    if(!ret){
        MALLOC_ERROR("key copy failed");
        return NULL;
    }
    init_key(ret, o->v, o->n);
    return ret;
}

/**
 * @brief initializes a key struct
 *
 * @param p prime
 * @param q prime
 * @return void
 */
void init_pair_keys(Key* pKey, Key* sKey, int low_size, int up_size){
    assert(low_size < up_size);

    int64 p, q, n, s, u;
    int try=0;
    do{
        p = random_prime_number(low_size, up_size, 5000);
        q = random_prime_number(low_size, up_size, 5000);
        if(p == q || p == -1 || q == -1) continue;
        if(++try > 1000){
            // possible si low_size est trop grand
            printf("ERROR: couldn't generate public-private key pair!");
            return;
        }
    }
    while(!generate_key_values(p, q, &n, &s, &u));
    init_key(pKey, s, n);
    init_key(sKey, u, n);
}

/**
 * @brief converts a Key to string of the form " (v, n) "
 *
 * @param Key key to convert
 * @return char* : allocated on the heap
 */
char* key_to_str(const Key* key){
    assert(key);
    char buff[250];
    sprintf(buff, "(%llx, %llx)", key->v, key->n);
    return strdup(buff);
}

/**
 * @brief parses a string of the form " (v, n) " into a Key struct
 *
 * @param char string to convert
 * @return Key* : allocated on the heap
 */
Key* str_to_key(const char* str){
    assert(str);
    int64 v, n;
    if(2 != sscanf(str, " ( %llx , %llx ) ", &v, &n))
        return NULL;
    Key* ret = malloc(sizeof(Key));
    if(!ret){
        MALLOC_ERROR("stro to key failed");
        return NULL;
    }
    init_key(ret, v, n);
    return ret;
}

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

/**
 * @brief encrypt a message using
 * modular exponentiation and a public RSA key (s,n).
 * 
 * @param chaine message to be crypted
 * @param s 
 * @param n 
 * @return int64* : encrypted message
 */
int64* encrypt(const char* chaine, int64 s, int64 n) {
    int N = strlen(chaine);
    int64* ret = malloc(sizeof(int64) * (N + 1));
    if(!ret){
        MALLOC_ERROR("encryption failed!");
        return NULL;
    }
    ret[N] = '\0';
    for (int i = 0; i < N; i++) {
        int m = chaine[i];
        ret[i] = modpow(m, s, n);
    }
    return ret;
}

/**
 * @brief decrypt a message using modular
 * exponentiation and a private RSA key (u,n).
 * 
 * @param crypted message to be decrypted
 * @param size 
 * @param u 
 * @param n 
 * @return char* : original message
 */
char* decrypt(const int64* crypted, int size, int64 u, int64 n) {
    char* ret = (char*)malloc(sizeof(char)*(size+1));
    if(!ret){
        MALLOC_ERROR("decryption failed!");
        return NULL;
    }
    ret[size] = '\0';
    for(int i=0; i<size; i++){        
        int64 c = crypted[i];
        ret[i] = modpow(c,u,n);
    }
    return ret;
}
