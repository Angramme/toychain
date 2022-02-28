#include "test/test.h"
#include "lib/fastexp.h"
#include "clocktime.h"

int main(){

    //Test de validite
    TEST(modpow_naive(3, 3, 11), 5);
    TEST(modpow_naive(2, 10, 69), 58);

    TEST(modpow(3, 3, 11), 5);
    TEST(modpow(2, 10, 69), 58);

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