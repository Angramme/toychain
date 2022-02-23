#include "test.h"
#include "clocktime.h"
#include "../lib/prime.h"

int main(){
    TEST(is_prime_naive(7), true);
    TEST(is_prime_naive(248389), true);
    TEST(is_prime_naive(248389 + 2), false);

    TEST_SUMMARY();


    // double dur = 0;
    // // long num = 200000000;
    // long num = 290001931;
    // // 290006657
    // long bigg = 0;
    // getchar();
    // while(dur < 2){
    //     printf("testing %ld \n", num);
    //     if(bigg) printf("biggest so far heh %ld \n", bigg);
    //     bool ret;
    //     CLOCK_TIME(dur, ret = is_prime_naive(num));
    //     if(ret && dur <= 2){
    //         bigg = num;
    //         printf("  took %f \n", dur);
    //     }
    //     if(dur > 2){
    //         break;
    //     }
    //     num ++;
    // }
    // printf("plus gros prime est lala %ld \n", bigg);
}