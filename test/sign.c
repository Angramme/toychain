#include "lib/sign.h"
#include "test/test.h"
#include "lib/types.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int64* garbage(int len){
    int64* con = malloc(sizeof(int64)*(len+1));
    con[len] = '\0';
    for(int i=0; i<len; i++){
        con[i] = rand() % 8000000;
    }
    return con;
}

void test_signature_to_str_to_signature(){
    int len = rand() % 250;
    int64* con = garbage(len);
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

void test_sign_verify(){
    int len = rand()%500;
    char* msg = malloc(sizeof(char)*(len+1));
    for(int i=0; i<len; i++) msg[i] = (char)rand_int64('a', 'z');
    msg[len] = '\0';

    Key pk, sk;
    init_pair_keys(&pk, &sk, 8, 12);

    Signature* sig = sign(msg, &sk);
    Protected* pro = init_protected(&pk, msg, sig);
    if(sig == NULL || pro == NULL) TEST_MSG(0, 1, "expected a valid return value");

    TEST(verify(pro), true);

    free(msg);
    free_signature(sig);
    free(pro); // free_protected would crash as signature is already freed and keys are not mallocd
}

void test_protected_to_str_str_to_protected(){
    Key* pkey = malloc(sizeof(Key));
    Key* skey = malloc(sizeof(Key));
    init_pair_keys(pkey, skey, 8, 12);

    int len = rand()%500;
    char* msg = malloc(sizeof(char)*(len+1));
    for(int i=0; i<len; i++) msg[i] = (char)rand_int64('a', 'z');
    msg[len] = '\0';

    Protected* pro = init_protected(
        pkey, 
        msg, 
        sign(msg, skey)
        );

    char* ps = protected_to_str(pro);
    Protected* pro2 = str_to_protected(ps);

    TEST(strcmp(pro->msg, pro2->msg), 0);
    TEST(pro->pKey->v, pro2->pKey->v);
    TEST(pro->pKey->n, pro2->pKey->n);
    TEST(pro->sig->len, pro2->sig->len);
    TEST(pro->sig->len, len);
    bool good = true;
    for(int i=0; i<len+1; i++) good = good && (pro->sig->xs[i] == pro2->sig->xs[i]);
    TEST_MSG(good, true, "the protected signatures differ!");

    free_protected(pro);
    free_protected(pro2);
    free(ps);
    free(skey);
}


int main(){
    TEST_SECTION(signature_to_str and str_to_signature);
    for(int i=0; i<10; i++){
        test_signature_to_str_to_signature();
    }
    TEST_SECTION_END();

    TEST_SECTION(sign and verify);
    for(int i=0; i<10; i++){
        test_sign_verify();
    }
    TEST_SECTION_END();

    TEST_SECTION(protected_to_str and str_to_protected);
    for(int i=0; i<10; i++){
        test_protected_to_str_str_to_protected();
    }
    TEST_SECTION_END();

    TEST_SUMMARY();
}