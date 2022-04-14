#include "lib/blockchain.h"
#include "test/test.h"
#include "test/clocktime.h"
#include "lib/dataio.h"
#include "lib/rsa.h"
#include "lib/error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void test_write_read_block(){
    Key* nkey = malloc(sizeof(Key));
    if(!nkey){
        MALLOC_ERROR("failed nkey allocation");
        return;
    }
    init_key(nkey, rand()%1000+1, rand()%1000+1);
    uint8* hash = hash_string("pipipopo");
    if(!hash){
        fprintf(stderr, "hash is NULL\n");
        free(nkey);
        return;
    }
    uint8* previous_hash = hash_string("rirififiloulou");
    if(!previous_hash){
        fprintf(stderr, "previous_hash is NULL\n");
        free(nkey);
        free(hash);
        return;
    }
    int nonce = rand()%200;
    CellProtected* list_p1 = rand_list_protected(6);
    if(!list_p1){
        fprintf(stderr, "list_p1 is NULL\n");
        free(nkey);
        free(hash);
        free(previous_hash);
        return;
    }

    Block* nblock = malloc(sizeof(Block));
    if(!nblock){
        MALLOC_ERROR("failed nblock allocation");
        free(nkey);
        free(hash);
        free(previous_hash);
        free_list_protected(list_p1);
        return;
    }
    nblock->author = nkey;
    nblock->previous_hash = previous_hash;
    nblock->hash = hash;
    nblock->nonce = nonce;
    nblock->votes = list_p1;
    
    write_block("../temp/blocktest.txt", nblock);
    Block* rblock = read_block("../temp/blocktest.txt");
    if(!rblock){
        fprintf(stderr, "rblock is NULL\n");
        free_block(nblock);
        return;
    }

    char* p1_tochar = list_protected_to_str(nblock->votes);
    char* p2_tochar = list_protected_to_str(rblock->votes);

    char* nhash = hash_to_str(nblock->hash);
    char* nphash = hash_to_str(nblock->previous_hash);
    char* rhash = hash_to_str(rblock->hash);
    char* rphash = hash_to_str(rblock->previous_hash);

    if(!p1_tochar || !p2_tochar || !nhash || !nphash || !rhash || !rphash){
        fprintf(stderr, "list protected to str or hash to str conversion failed\n");
        if(p1_tochar) free(p1_tochar);
        if(p2_tochar) free(p2_tochar);
        if(nhash) free(nhash);
        if(nphash) free(nphash);
        if(rhash) free(rhash);
        if(rphash) free(rphash);
        free_block(nblock);
        free_block(rblock);
        return;
    }
    
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

void performance_tests(){
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

void question96(){
    // 1
    const size_t nV = 1000, nC = 5;
    // generate_random_data(nV, nC, BLOCK_STORAGE_DIR);
    
    {
        // 2
        CellProtected* decl = read_protected(BLOCK_STORAGE_DIR "declarations.txt");
        CellKey* citoyens = read_public_keys(BLOCK_STORAGE_DIR "keys.txt");
        size_t i;
        CellProtected* cur = decl;
        
        // 3
        CellTree* blocktree = NULL; 
        const int d = 1;
        const size_t interval = 50;
        i=0;
        while(cur){
            submit_vote(cur->data);
            i++;
            if(i!=0 && i%interval==0){
                create_block(&blocktree, cur->data->pKey, d);
                Block* B = read_block(PENDING_BLOCK_FILE);
                if(!B){
                    printf("error reading block! \n");
                    free_tree(blocktree);
                    free_list_protected(decl);
                    free_list_keys(citoyens);
                    return;
                }
                char* name = hash_to_str(B->hash);
                add_block(d, name);
                free(name);
                free_block(B);
            }
            cur = cur->next;
        }

        // 3.5
        printf("arbre original: \n");
        print_tree(blocktree);

        free_tree(blocktree);
        free_list_protected(decl);
        free_list_keys(citoyens);
    }
    {
        // 4 
        CellTree* tr = read_tree();
        printf("\n\n arbre chargé: \n");
        print_tree(tr);

        CellKey* candidates = read_public_keys(BLOCK_STORAGE_DIR "candidates.txt");
        CellKey* voters = read_public_keys(BLOCK_STORAGE_DIR "keys.txt");

        Key* winner = compute_winner_BT(tr, candidates, voters, nC, nV);
        char* kstr = key_to_str(winner);
        printf("WINNER: %s \n", kstr);
        free(kstr);
        free(winner);

        free_list_keys(candidates);
        free_list_keys(voters);

        free_tree(tr);
    }
}


int main(){
    TEST_SECTION(write_block and read_block);
    srand(time(NULL));
    int j;
    for(j=0; j<15; j++){
        test_write_read_block();
    }
    TEST_SECTION_END();


    TEST_SECTION(hash_to_str str_to_hash compare_hash copy_hash)
    {
        const char* str = "Hahah drole";
        uint8* hash = hash_string(str);
        if(!hash){
            fprintf(stderr, "hash is NULL\n");
            return -1;
        }
        char* hashstr = hash_to_str(hash);
        if(!hashstr){
            fprintf(stderr, "hashstr is NULL\n");
            free(hash);
            return -1;
        }
        uint8* hash2;
        TEST(str_to_hash(hashstr, &hash2), true);
        if(!hash2){
            fprintf(stderr, "hash2 is NULL\n");
            free(hash);
            free(hashstr);
            return -1;
        }
        bool good = true;
        size_t i;
        for(i=0; (i<BLOCK_HASH_SIZE) && good; i++)
            good = good && (hash[i] == hash2[i]);
        TEST_MSG(compare_hash(hash, hash2), good, "compare hash function is incorrect");
        TEST_MSG(good, true, "the hashes differ!");
        if(!good){
            print_sha256_hash(hash);
            print_sha256_hash(hash2);
        }


        uint8* hash3 = copy_hash(hash);
        if(!hash3){
            fprintf(stderr, "hash3 is NULL\n");
            free(hash);
            free(hash2);
            free(hashstr);
            return -1;
        }
        TEST(compare_hash(hash, hash3), true);

        free(hash);
        free(hash2);
        free(hash3);
        free(hashstr);
    }
    TEST_SECTION_END()

    TEST_SECTION(compute_proof_of_work verify_block);
    {
        const int d = 4;
        Key pk, sk;
        init_pair_keys(&pk, &sk, 8, 12);
        uint8 prev_hash[BLOCK_HASH_SIZE];
        size_t i;
        for(i=0; i<BLOCK_HASH_SIZE; i++) prev_hash[i] = (uint8)rand()%255;
        CellProtected* decls = rand_list_protected_range(5, 'a', 'z');
        if(!decls){
            fprintf(stderr, "delcs is NULL\n");
            return -1;
        }
        Block* b = init_block(&pk, decls, prev_hash);
        if(!b){
            fprintf(stderr, "b is NULL\n");
            free_list_protected(decls);
            return -1;
        }
        free_list_protected(decls);
        
        TEST_MSG(verify_block(b, d), false, "we either got very lucky or the verify_block function is incorrrect...");
        compute_proof_of_work(b, d);
        TEST_MSG(verify_block(b, d), true, "either verify_block or compute_proof_of_work is incorrect...");
        
        printf("hash with %d zeros: ", d);
        print_sha256_hash(b->hash);

        free_block(b);
    }
    TEST_SECTION_END();

    TEST_SECTION(question 9.6);
    question96();
    TEST_SECTION_END();

    // TEST_SECTION(performance tests);
    // performance_tests();
    // TEST_SECTION_END();

    TEST_SUMMARY();
}