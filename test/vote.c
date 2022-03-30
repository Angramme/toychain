#include "lib/vote.h"
#include "lib/sign.h"
#include "lib/rsa.h"
#include "test/test.h"

int main(){

    TEST_SECTION(create_hashcell free_hashcell);
    {
        Key k;
        init_key(&k, rand_int64(0, 42424269), rand_int64(0, 42424269));
        HashCell* hc = create_hashcell(&k);
        free_hashcell(hc);
    }
    TEST_SECTION_END();

    TEST_SECTION(create_hashtable);
    {
        // create_hashtable()
    }
    TEST_SECTION_END();

    TEST_SECTION(hash_function);
    {
        for(int I=0; I<5; I++){
            size_t size = rand_int64(10, 1000);
            
            Key k;
            init_key(&k, 0, 0);
            for(int i=0; i<100; i++){
                k.n = rand_int64(0, 42424269);
                k.v = rand_int64(0, 42424269);

                uint32 x = hash_function(&k, size);
                TEST_MSG(x >= 0 && x < size, true, "x is not contained in the proper range");
            }
        }

    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}