#include "lib/rsa.h"
#include "test.h"
#include <string.h>
#include <stdlib.h>


void test_random_prime_number() {
    long p = random_prime_number(15, 16, 5000);
    long q = random_prime_number(15, 16, 5000);
    long n, s, u;
    generate_key_values(p, q, &n, &s, &u);

    long U, V;
    TEST_MSG(extended_gcd(p, q, &U, &V), 1, "pgcd of p and q is not 1!");
    TEST_MSG((p*u)%q, 1, "the p*u%q is not 1!");
}

void test_rsa_encryption_decryption(char* message) {
    long p = random_prime_number(15, 16, 5000);
    long q = random_prime_number(15, 16, 5000);
    long n, s, u;
    generate_key_values(p, q, &n, &s, &u);

    int len = strlen(message);

    //Chiffrement:
    long* crypted = encrypt(message, s, n);

    //Dechiffrement
    char* decoded = decrypt(crypted, len, u, n);

    int strcmpres = strcmp(message, decoded);
    TEST_MSG(strcmpres, 0, "the encoded and decoded messages are different!");
    if(strcmpres != 0){
        printf("original  > \"%s\" \n", message);
        printf("decoded   > \"%s\" \n", decoded);
        printf("encypted  > ");
        for(int i=0; i<len; i++) printf("%ld, ", crypted[i]);
        printf(" \n");
    }

    free(crypted);
    free(decoded);
}

int main() {
    srand(424242);

    for(int i=0; i<5; i++)
        test_random_prime_number();

    for(int i=0; i<5; i++){
        int len = rand_long(1, 10);
        char* msg = malloc(sizeof(char)*(len+1));
        msg[len] = '\0';
        for(int j=0; j<len; j++) 
            msg[j] = rand_long('a', 'z');

        test_rsa_encryption_decryption(msg);

        free(msg);
    }

    TEST_SUMMARY();
}