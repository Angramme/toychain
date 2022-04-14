#include "test/test.h"
#include "test/clocktime.H"
#include "lib/blockchain.h"

#include <stdlib.h>
#include <stdio.h>

void blockchain_performance_tests(){
    Key pk, sk;
    init_pair_keys(&pk, &sk, 8, 12);
    uint8 prev_hash[BLOCK_HASH_SIZE];
    for(size_t i=0; i<BLOCK_HASH_SIZE; i++) prev_hash[i] = (uint8)rand()%255;
    CellProtected* decls = rand_list_protected_range(42, 'a', 'z');
    Block* b = init_block(&pk, decls, prev_hash);
    free_list_protected(decls);

    for(int d=1; d<BLOCK_HASH_SIZE/2; d++){
        double secs;
        CLOCK_TIME(secs, compute_proof_of_work(b, d));
        printf("%d %f \n", d, secs);
        if(secs > 10) break;
    }

    free_block(b);
}

void mmath_performance_tests(){
    double dur_naiv;
    double dur;
    FILE *f = fopen("plotfastexp1.txt", "w");
    int m;
    for(m = 1; m <=200000000; m*=2){
        CLOCK_TIME(dur_naiv, modpow_naive(9435000329,m,1093));
        CLOCK_TIME(dur, modpow(3,m,10));
        fprintf(f, "%d %f %f\n", m, dur_naiv, dur);
    }
    fclose(f);
}

int main(){
    #if PERFTESTS==0
    printf("Performance tests were skipped. Please specify \"PERFTESTS=1\" as \"make test\" argument to execute them...\n");
    return 0;
    #endif

    TEST_SECTION(blockchain performance tests);
    blockchain_performance_tests();
    TEST_SECTION_END();

    TEST_SECTION(mmath performance tests)
    mmath_performance_tests();
    TEST_SECTION_END();

    TEST_SUMMARY();
}