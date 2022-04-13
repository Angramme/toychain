#include "lib/rsa.h"
#include "test/test.h"
#include "lib/error.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void print_int64tab(int64* tab, int size){
    printf("[");
    int i;
    for(i=0; i<size; i++){
        if(i == size-1){
            printf("%lld", tab[i]);
        } else {
            printf("%lld, ", tab[i]);
        }
    }
    printf("]\n");
}

void test_generate_key_values() {
    int64 p, q, n, s, u;
    do{
        p = random_prime_number(15, 16, 5000);
        q = random_prime_number(15, 16, 5000);
        if(p == q) continue;
    }
    while(!generate_key_values(p, q, &n, &s, &u));

    int64 U, V;
    TEST_MSG(extended_gcd(p, q, &U, &V), 1, "pgcd of p and q is not 1!");
    TEST_MSG(((p*U)%q + q) % q, 1, "the p*u mod q is not 1!");
}

void test_rsa_encryption_decryption(char* message) {
    int64 p, q, n, s, u;
    do{
        p = random_prime_number(15, 16, 5000);
        q = random_prime_number(15, 16, 5000);
    }
    while(!generate_key_values(p, q, &n, &s, &u));

    if(u < 0 || s < 0 || n < 0){
        TEST_MSG(u < 0 || s < 0 || n < 0, 0, "aborting the rest of the tests!");
        return;
    }

    int len = strlen(message);

    //Chiffrement:
    int64* crypted = encrypt(message, s, n);
    if(!crypted){
        fprintf(stderr, "crypted is NULL\n");
        return;
    }
    //Dechiffrement
    char* decoded = decrypt(crypted, len, u, n);
    if(!decoded){
        fprintf(stderr, "decoded is NULL\n");
        free(crypted);
        return;
    }

    int strcmp_res = strcmp(message, decoded);
    TEST_MSG(strcmp_res, 0, "the encoded and decoded messages are different!");
    if(strcmp_res != 0){
        // print more error detail:
        printf("public key = (%lld %lld) ; private key = (%lld %lld) \n", s, n, u, n);
        printf("original > \"%s\" \n", message);
        printf("decoded  > \"%s\" \n", decoded);
        printf("crypted  > ");
        print_int64tab(crypted,len);
        printf(" \n");
    }

    free(crypted);
    free(decoded);
}

void test_key_str_conversion(){
    Key k;
    init_key(&k, rand()%800000, rand()%800000);

    char* str = key_to_str(&k);
    if(!str){
        fprintf(stderr, "str is NULL\n");
        return;
    }
    Key* key = str_to_key(str);
    if(!key){
        fprintf(stderr, "key is NULL\n");
        free(str);
        return;
    }

    TEST(k.v, key->v);
    TEST(k.n, key->n);

    free(str);
    free(key);
}

int main() {
    srand(time(NULL));
    TEST_SECTION(generate_key_values);
    int i;
    for(i=0; i<5; i++)
        test_generate_key_values();
    TEST_SECTION_END();

    TEST_SECTION(encrypt and decrypt);
    // with fixed message
    for(i=0; i<5; i++){
        test_rsa_encryption_decryption("Toutes les voitures sont stupides en ville.");
    }
    // with random message
    for(i=0; i<8; i++){
        int len = rand_int64(1, 10);
        char* msg = malloc(sizeof(char)*(len+1));
        if(!msg){
            MALLOC_ERROR("failed msg allocation");
            return -1;
        }

        msg[len] = '\0';
        int j;
        for(j=0; j<len; j++){
            do{ msg[j] = (char)rand_int64(0, 255); }
            while(msg[j] != '\0');
        }

        test_rsa_encryption_decryption(msg);
        free(msg);
    }
    TEST_SECTION_END();

    TEST_SECTION(str_to_key and key_to_str)
    for(i=0; i<10; i++){
        test_key_str_conversion();
    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}