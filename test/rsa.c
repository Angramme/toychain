#include "../lib/rsa.h"
#include "test.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void print_int64tab(int64* tab, int size){
    printf("[");
    for(int i=0; i<size; i++){
        if(i == size-1){
            printf("%ld", (long)tab[i]);
        } else {
            printf("%ld ", (long)tab[i]);
        }
    }
    printf("]\n");
}

void test_generate_key_values() {
    int64 p = random_prime_number(15, 16, 5000);
    int64 q = random_prime_number(15, 16, 5000);
    TEST(is_prime_miller(p, 100), true);
    TEST(is_prime_miller(q, 100), true);
    assert(p != q);
    int64 n, s, u;
    generate_key_values(p, q, &n, &s, &u);

    int64 U, V;
    TEST_MSG(extended_gcd(p, q, &U, &V), 1, "pgcd of p and q is not 1!");
    TEST_MSG(((p*U)%q + q) % q, 1, "the p*u mod q is not 1!");
}

void test_rsa_encryption_decryption(char* message) {
    int64 p = random_prime_number(15, 16, 5000);
    int64 q = random_prime_number(15, 16, 5000);
    int64 n, s, u;
    generate_key_values(p, q, &n, &s, &u);

    if(u < 0 || s < 0 || n < 0){
        TEST_MSG(u < 0 || s < 0 || n < 0, 0, "aborting the rest of the tests!");
        return;
    }

    int len = strlen(message);

    //Chiffrement:
    int64* crypted = encrypt(message, s, n);

    //Dechiffrement
    char* decoded = decrypt(crypted, len, u, n);

    int strcmp_res = strcmp(message, decoded);
    TEST_MSG(strcmp_res, 0, "the encoded and decoded messages are different!");
    if(strcmp_res != 0){
        printf("original > \"%s\" \n", message);
        printf("decoded  > \"%s\" \n", decoded);
        printf("crypted  > ");
        print_int64tab(crypted,len);
        printf(" \n");
    }

    free(crypted);
    free(decoded);
}

int main() {
    srand(time(NULL));
    TEST_SECTION(generate_key_values);
    for(int i=0; i<5; i++)
        test_generate_key_values();
    TEST_SECTION_END();

    TEST_SECTION(encrypt and decrypt);
    for(int i=0; i<5; i++){
        int len = rand_int64(1, 10);
        char* msg = malloc(sizeof(char)*(len+1));
        //char msg[len+1];
        *(msg+len) = '\0';
        for(int j=0; j<len; j++) 
            *(msg+j) = rand_int64('a', 'z');

        test_rsa_encryption_decryption(msg);

        free(msg);
    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}