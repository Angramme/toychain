#include "lib/vote.h"
#include "test/test.h"
#include "lib/types.h"
#include <stdlib.h>
#include <stdbool.h>

void test_signature_to_str_to_signature(){
    int len = rand() % 250;
    int64* con = malloc(sizeof(int64)*(len+1));
    con[len] = '\0';
    for(int i=0; i<len; i++){
        con[i] = rand() % 8000000;
    }
    Signature* S = init_signature(con, len);

    char* str = signature_to_str(S);
    Signature* sig = str_to_signature(str);

    bool good = true;
    for(int i=0; i<len+1; i++){
        good = good && (sig->xs[i] == S->xs[i]);
    }
    TEST_MSG(good, true, "the converted and original signatures are different!");

    free(str);
    free_signature(sig);

    free_signature(S);
}

int main(){
    TEST_SECTION(signature_to_str and str_to_signature);
    for(int i=0; i<10; i++){
        test_signature_to_str_to_signature();
    }
    TEST_SECTION_END();
}