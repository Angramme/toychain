#include "lib/blockchain.h"
#include "test/test.h"
#include <stdbool.h>
#include <stdlib.h>

int main(){
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

    TEST_SUMMARY();
}