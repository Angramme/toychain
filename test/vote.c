#include "lib/vote.h"
#include "lib/sign.h"
#include "lib/rsa.h"
#include "test/test.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

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
        //as there is no way to check if the winner is valid, we create an election in test/comput_winner_data,
        //in which we know who is going to win.
        CellKey* vots = read_public_keys("./compute_winner_data/keys.txt");
        CellKey* cands = read_public_keys("./compute_winner_data/candidates.txt");
        CellProtected* decl = read_protected("./compute_winner_data/declarations.txt");
        Key* win = compute_winner(decl, cands, vots, 5, 10);
        char* strw = key_to_str(win);
        char true_winner[20] = "(ba8f5, 2b537f)";
        //we know the public key of the winner should be (ba8f5, 2b537f).
        TEST_MSG(strcmp(strw, true_winner), 0, "compute_winner failed to find winner");
        free_list_keys(vots);
        free_list_keys(cands);
        free_list_protected(decl);
        free(win);
        free(strw);

        /*
        //election simulation (doesn't test is winner is correct)
        int i;
        for(i=0; i<5; i++){
            int sizeV = 100;
            int sizeC = 6;
            sizeC = sizeC < sizeV ? sizeC : sizeV-1;
            generate_random_data(sizeV, sizeC, "../data/temp/");
            CellKey* voters = read_public_keys("../data/temp/keys.txt");
            CellKey* candidates = read_public_keys("../data/temp/candidates.txt");
            CellProtected* declarations = read_protected("../data/temp/declarations.txt");
            Key* winner = compute_winner(declarations, candidates, voters, sizeC, sizeV);
            char* strwin = key_to_str(winner);
            printf("winner is : %s\n", strwin);

            free_list_keys(voters);
            free_list_keys(candidates);
            free_list_protected(declarations);
            free(winner);
            free(strwin);
        }*/
    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}