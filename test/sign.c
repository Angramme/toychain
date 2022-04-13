#include "lib/sign.h"
#include "test/test.h"
#include "lib/types.h"
#include "lib/error.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TEMP_DIRECTORY "../temp"

int64* garbage(int len){
    int64* con = malloc(sizeof(int64)*(len+1));
    if(!con){
        MALLOC_ERROR("failed con allocation");
        return NULL;
    }
    con[len] = '\0';
    int i;
    for(i=0; i<len; i++){
        con[i] = rand() % 8000000;
    }
    return con;
}

void test_signature_to_str_to_signature(){
    int len = rand() % 250;
    int64* con = garbage(len);
    if(!con)return;
    Signature* S = init_signature_raw(con, len);
    if(!S){
        free(con);
        return;
    }

    char* str = signature_to_str(S);
    if(!str){
        free_signature(S);
        return;
    }
    Signature* sig = str_to_signature(str);
    if(!sig){
        free_signature(S);
        free(str);
        return;
    }

    bool good = true;
    int i;
    for(i=0; i<len+1; i++){
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
    if(!msg){
        MALLOC_ERROR("failed msg allocation");
        return;
    }
    int i;
    for(i=0; i<len; i++) msg[i] = (char)rand_int64('a', 'z');
    msg[len] = '\0';

    Key* pk = malloc(sizeof(Key));
    if(!pk){
        MALLOC_ERROR("failed pk allocation");
        free(msg);
        return;
    }
    Key* sk = malloc(sizeof(Key));
    if(!sk){
        MALLOC_ERROR("failed sk allocation");
        free(pk);
        free(msg);
        return;
    }
    init_pair_keys(pk, sk, 8, 12);


    Signature* sig = sign(msg, sk);
    if(!sig){
        MALLOC_ERROR("failed sig allocation");
        free(pk);
        free(sk);
        free(msg);
        return;
    }
    Protected* pro = init_protected_raw(pk, msg, sig); // Protected takes ownership of signature, message and public key
    if(!pro){
        MALLOC_ERROR("failed pro allocation");
        free(pk);
        free(sk);
        free(msg);
        free(sig);
        return;
    }
    if(sig == NULL || pro == NULL) TEST_MSG(0, 1, "expected a valid return value");

    TEST(verify(pro), true);

    free_protected(pro);
    free(sk);
}

void test_protected_to_str_str_to_protected(){
    Key* pkey = malloc(sizeof(Key));
    if(!pkey){
        MALLOC_ERROR("failed pkey allocation");
        return;
    }
    Key* skey = malloc(sizeof(Key));
    if(!skey){
        MALLOC_ERROR("failed skey allocation");
        free(pkey);
        return;
    }
    init_pair_keys(pkey, skey, 8, 12);

    int len = rand()%500 + 1;
    char* msg = malloc(sizeof(char)*(len+1));
    if(!msg){
        MALLOC_ERROR("failed msg allocation");
        free(pkey);
        free(skey);
        return;
    }
    int i;
    for(i=0; i<len; i++) msg[i] = (char)rand_int64('a', 'z');
    msg[len] = '\0';

    Protected* pro = init_protected_raw(
        pkey, 
        msg, 
        sign(msg, skey)
        );
    if(!pro){
        MALLOC_ERROR("failed pro allocation");
        free(pkey);
        free(skey);
        free(msg);
        return;
    }

    char* ps = protected_to_str(pro);
    if(!ps){
        MALLOC_ERROR("failed ps allocation");
        free_protected(pro);
        free(skey);
        return;
    }
    Protected* pro2 = str_to_protected(ps);
    if(!pro2){
        MALLOC_ERROR("failed pro2 allocation");
        free_protected(pro);
        free(skey);
        free(ps);
        return;
    }

    TEST(strcmp(pro->msg, pro2->msg), 0);
    TEST(pro->pKey->v, pro2->pKey->v);
    TEST(pro->pKey->n, pro2->pKey->n);
    TEST(pro->sig->len, pro2->sig->len);
    TEST(pro->sig->len, len);
    bool good = true;
    for(i=0; i<len+1; i++) good = good && (pro->sig->xs[i] == pro2->sig->xs[i]);
    TEST_MSG(good, true, "the protected signatures differ!");

    free_protected(pro);
    free_protected(pro2);
    free(ps);
    free(skey);
}

int main(){
    TEST_SECTION(signature_to_str and str_to_signature);
    int i;
    for(i=0; i<10; i++){
        test_signature_to_str_to_signature();
    }
    TEST_SECTION_END();

    TEST_SECTION(sign and verify);
    for(i=0; i<10; i++){
        test_sign_verify();
    }
    TEST_SECTION_END();

    TEST_SECTION(protected_to_str and str_to_protected);
    for(i=0; i<10; i++){
        test_protected_to_str_str_to_protected();
    }
    TEST_SECTION_END();

    TEST_SUMMARY();

}