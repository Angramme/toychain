#include "lib/blockchain.h"
#include "test/test.h"
#include "test/clocktime.h"
#include "lib/dataio.h"
#include "lib/rsa.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void test_write_read_block(){
    Key* nkey = malloc(sizeof(Key));
    init_key(nkey, rand()%1000+1, rand()%1000+1);
    uint8* hash = str_to_hash("pipipopo");
    uint8* previous_hash = str_to_hash("rirififiloulou");
    int nonce = rand()%200;
    CellProtected* list_p1 = rand_list_protected(6);

    Block* nblock = malloc(sizeof(Block));
    nblock->author = nkey;
    nblock->previous_hash = previous_hash;
    nblock->hash = hash;
    nblock->nonce = nonce;
    nblock->votes = list_p1;
    
    write_block("../temp/blocktest.txt", nblock);
    Block* rblock = read_block("../temp/blocktest.txt");
    char* p1_tochar = list_protected_to_str(nblock->votes);
    char* p2_tochar = list_protected_to_str(rblock->votes);

    char* nhash = hash_to_str(nblock->hash);
    char* nphash = hash_to_str(nblock->previous_hash);
    char* rhash = hash_to_str(rblock->hash);
    char* rphash = hash_to_str(rblock->previous_hash);
    
    bool flag = true;
    flag = flag && (rblock->author->n == nblock->author->n);
    flag = flag && (rblock->author->v == nblock->author->v);
    flag = flag && (strcmp(nhash, rhash) == 0);
    flag = flag && (strcmp(nphash, rphash) == 0);
    flag = flag && (rblock->nonce == nblock->nonce);
    TEST_MSG(flag, true, "different results");

    free(nhash);
    free(nphash);
    free(rhash);
    free(rphash);
    free(p1_tochar);
    free(p2_tochar);
    free_block(nblock);
    free_block(rblock);
}


int main(){
    TEST_SECTION(write_block and read_block);
    srand(time(NULL));
    for(int i=0; i<15; i++){
        test_write_read_block();
    }
    TEST_SECTION_END();


    TEST_SECTION(hash_to_str str_to_hash compare_hash copy_hash)
    {
        const char* str = "Hahah drole";
        uint8* hash = hash_string(str);
        char* hashstr = hash_to_str(hash);
        uint8* hash2 = str_to_hash(hashstr);
        bool good = true;
        for(size_t i=0; (i<BLOCK_HASH_SIZE) && good; i++)
            good = good && (hash[i] == hash2[i]);
        TEST_MSG(compare_hash(hash, hash2), good, "compare hash function is incorrect");
        TEST_MSG(good, true, "the hashes differ!");
        if(!good){
            print_sha256_hash(hash);
            print_sha256_hash(hash2);
        }


        uint8* hash3 = copy_hash(hash);
        TEST(compare_hash(hash, hash3), true);

        free(hash3);
        free(hashstr);
        free(hash2);
    }
    TEST_SECTION_END()

    TEST_SECTION(compute_proof_of_work verify_block);
    {
        const int d = 4;
        Key pk, sk;
        init_pair_keys(&pk, &sk, 8, 12);
        uint8 prev_hash[BLOCK_HASH_SIZE];
        for(size_t i=0; i<BLOCK_HASH_SIZE; i++) prev_hash[i] = (uint8)rand()%255;
        CellProtected* decls = rand_list_protected_range(5, 'a', 'z');
        Block* b = init_block(&pk, decls, prev_hash);
        free_list_protected(decls);
        
        TEST_MSG(verify_block(b, d), false, "we either got very lucky or the verify_block function is incorrrect...");
        compute_proof_of_work(b, d);
        TEST_MSG(verify_block(b, d), true, "either verify_block or compute_proof_of_work is incorrect...");
        
        printf("hash with %d zeros: ", d);
        print_sha256_hash(b->hash);

        free_block(b);
    }
    TEST_SECTION_END();

    // TEST_SECTION(performance tests);
    // {
    //     Key pk, sk;
    //     init_pair_keys(&pk, &sk, 8, 12);
    //     uint8 prev_hash[BLOCK_HASH_SIZE];
    //     for(size_t i=0; i<BLOCK_HASH_SIZE; i++) prev_hash[i] = (uint8)rand()%255;
    //     CellProtected* decls = rand_list_protected_range(42, 'a', 'z');
    //     Block* b = init_block(&pk, decls, prev_hash);
    //     free_list_protected(decls);

    //     for(int d=1; d<BLOCK_HASH_SIZE/2; d++){
    //         double secs;
    //         CLOCK_TIME(secs, compute_proof_of_work(b, d));
    //         printf("%d %f \n", d, secs);
    //         if(secs > 10) break;
    //     }

    //     free_block(b);
    // }
    // TEST_SECTION_END();

    TEST_SUMMARY();
}