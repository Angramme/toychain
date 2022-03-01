#include "test/test.h"
#include "lib/mmath.h"
#include "clocktime.h"

int main(){

    TEST(modpow_naive(3, 3, 11), 5);
    TEST(modpow_naive(2, 10, 69), 58);

    TEST(modpow(3, 3, 1000), 27);
    TEST(modpow(3, 4, 10000), 81);
    TEST(modpow(2, 10, 10000), 1024);
    TEST(modpow(3, 3, 11), 5);
    TEST(modpow(2, 10, 69), 58);
    
    TEST(modpow_r(3, 3, 11), 5);
    TEST(modpow_r(2, 10, 69), 58);

    TEST(is_prime_naive(7), true);
    TEST(is_prime_naive(248389), true);
    TEST(is_prime_naive(248389 + 2), false);

    TEST_SUMMARY(); 
    

    //Test de performances
    /*
    double dur_naiv;
    double dur;
    FILE *f = fopen("plotfastexp1.txt", "w");
    for(int m = 1; m <=200000000; m*=2){
        CLOCK_TIME(dur_naiv, modpow_naive(9435000329,m,1093));
        CLOCK_TIME(dur, modpow(3,m,10));
        fprintf(f, "%d %f %f\n", m, dur_naiv, dur);
    }
    fclose(f);
    */
}