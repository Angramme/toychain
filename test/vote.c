#include "lib/vote.h"
#include "lib/sign.h"
#include "lib/rsa.h"
#include "test/test.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){
    srand(time(NULL));
    TEST_SECTION(create_hashcell free_hashcell);
    {
        Key k;
        init_key(&k, rand_int64(0, 42424269), rand_int64(0, 42424269));
        HashCell* hc = create_hashcell(&k);
        if(hc) free_hashcell(hc);
    }
    TEST_SECTION_END();

    TEST_SECTION(create_hashtable);
    {
        // create_hashtable()
    }
    TEST_SECTION_END();

    TEST_SECTION(hash_function);
    {   
        int I;
        for(I=0; I<5; I++){
            size_t size = rand_int64(10, 1000);
            
            Key k;
            init_key(&k, 0, 0);
            int i;
            for(i=0; i<100; i++){
                k.n = rand_int64(0, 42424269);
                k.v = rand_int64(0, 42424269);

                uint32 x = hash_function(&k, size);
                TEST_MSG(x >= 0 && x < size, true, "x is not contained in the proper range");
            }
        }

    }
    TEST_SECTION_END();

    TEST_SECTION(compute_winner);
    {
        int i;
        for(i=0; i<1; i++){
            int sizeV = rand()%100 + 10;
            int sizeC = rand()%10 + 2;
            sizeC = sizeC < sizeV ? sizeC : sizeV-1;
            generate_random_data(sizeV, sizeC, "../temp/");
            CellKey* voters = read_public_keys("../temp/keys.txt");
            CellKey* candidates = read_public_keys("../temp/candidates.txt");
            CellProtected* declarations = read_protected("../temp/declarations.txt");
            Key* winner = compute_winner(declarations, candidates, voters, sizeC, sizeV);
            printf("winner is (%lld %lld)\n", winner->n, winner->v);

            free_list_keys(voters);
            free_list_keys(candidates);
            free_list_protected(declarations);
            free(winner);
        }
    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}