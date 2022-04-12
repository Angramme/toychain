#include "lib/sign.h"
#include "lib/rsa.h"
#include "lib/error.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**
 * @file sign.c
 * @brief a collection of functions related to signing messages and the voting system
 * 
 */

/**
* @brief initialize a signaure without copying the data
* !!! takes ownership of content
* 
* @param content : content of the signature
* @param size : size of content
*
* @return Signature* : a new signature on the heap
*/
Signature* init_signature_raw(int64* content, int size){
    Signature* ret = malloc(sizeof(Signature));
    if(!ret){
        MALLOC_ERROR("couldn't initialize signature!");
        return NULL;
    }
    ret->xs = content;
    ret->len = size;
    return ret;
}

/**
* @brief initialize a signaure
*
* @param content : content of the signature
* @param size : size of content
*
* @return Signature* : a new signature on the heap
*/
Signature* init_signature(const int64* content, int size){
    int64* cpy = malloc(sizeof(int64)*(size+1));
    if(!cpy){
        MALLOC_ERROR("couldn't initialize signature");
        return NULL;
    }
    memcpy(cpy, content, sizeof(int64)*(size+1));
    return init_signature_raw(cpy, size);
}

Signature* copy_signature(const Signature* o){
    return init_signature(o->xs, o->len);
}

/**
* @brief free signature
*
* @param sig : signature to delete
*/
void free_signature(Signature* sig){
    free(sig->xs);
    free(sig);
}

/**
* @brief sign a message
*
* @param mess : message to sign
* @param sKey : private key to sign the message with
*
* @return Signature*
*/
Signature* sign(char* mess, Key* sKey){
    assert(mess && sKey);
    int64* con = encrypt(mess, sKey->v, sKey->n);
    int len = strlen(mess);
    return init_signature_raw(con, len);
}

/**
 * @brief create a signed declaration
 * !!! Takes ownership of key, message and signature
 * 
 * @param pKey : public key
 * @param mess : associated message
 * @param sgn : signature
 * @return Protected* 
 */
Protected* init_protected_raw(Key* pKey, char* mess, Signature* sgn){
    assert(pKey && mess && sgn);
    Protected* ret = malloc(sizeof(Protected));
    if(!ret){
        MALLOC_ERROR("coudln't initialize protected");
        return NULL;
    }
    ret->msg = mess;
    ret->pKey = pKey;
    ret->sig = sgn;
    return ret;
}

/**
 * @brief create a signed declaration
 * 
 * @param pKey : public key
 * @param mess : associated message
 * @param sgn : signature
 * @return Protected* 
 */
Protected* init_protected(Key* pKey, char* mess, Signature* sgn){
    assert(pKey && mess && sgn);
    Key* pkeycpy = copy_key(pKey);
    char* msgcpy = strdup(mess);
    Signature* sgncpy = copy_signature(sgn);
    return init_protected_raw(pkeycpy, msgcpy, sgncpy);
}

/**
 * @brief copy protected 
 * 
 * @param o 
 * @return Protected* 
 */
Protected* copy_protected(const Protected* o){
    return init_protected(o->pKey, o->msg, o->sig);
}

/**
 * @brief free a signed declaration and it's content
 * 
 * @param pr : element to free
 */
void free_protected(Protected* pr){
    free(pr->msg);
    free(pr->pKey);
    free_signature(pr->sig);
    free(pr);
}

/**
* @brief checks if the signature matches the message
*
* @param Protected* : protected message
*
* @return bool
*/
bool verify(const Protected* pr){
    assert(pr && pr->pKey);
    const int N = strlen(pr->msg);
    if(pr->sig->len != N) return false;
    char* truth = decrypt(pr->sig->xs, N, pr->pKey->v, pr->pKey->n);
    bool good = true;
    for(int i=0; i<N+1; i++){
        good = good && truth[i]==pr->msg[i];
        if(!good) break;
    }
    free(truth);
    return good;
}

/**
* @brief convert a signature to string
*
* @param sgn : signature to convert
*
* @return char* 
*/
char* signature_to_str(const Signature* sgn){
    assert(sgn->len > 0);
    const size_t maxsize = 10* sgn->len;
    char* result = malloc(maxsize * sizeof(char));
    if(!result){
        MALLOC_ERROR("coudln't convert signature to string");
        return NULL;
    }
    result[0]= '#';
    int pos = 1;
    char buffer [156];
    for(int i=0; i < sgn->len; i++) {
        sprintf(buffer, "%llx" , sgn->xs[i]);
        const size_t bufflen = strlen(buffer);
        assert(bufflen <= 9);
        for(int j=0; j<bufflen; j++) {
            assert(pos < maxsize);
            result[pos] = buffer[j];
            pos = pos +1;
        }
        result[pos] = '#';
        pos = pos +1;
    }
    result[pos] = '\0';
    result = realloc(result, (pos +1) * sizeof(char));
    return result;
}

/**
* @brief parse a string to a signature
*
* @param str
*
* @return Signature* 
*/
Signature* str_to_signature(const char* str){
    int len = strlen(str);
    int len2 = 0;
    while(
        len2 < len && 
        (
            (str[len2] >= 'a' && str[len2] <= 'f') || 
            (str[len2] >= '0' && str[len2] <= '9') ||
            (str[len2] == '#')
        )){
            len2++;
        }
    len = len < len2 ? len : len2;
    return str_to_signature_len(str, len);
}

Signature* str_to_signature_len(const char* str, const size_t len){
    size_t hash_count = 0;
    for(size_t i=0; i<len; i++) if(str[i] == '#') hash_count++;
    int64* content = (int64*)malloc(sizeof(int64) * hash_count);
    if(!content){
        MALLOC_ERROR("coudln't convert string to signature");
        return NULL;
    }
    int num = 0;
    char buffer [256];
    int pos = 0;
    for(int i=0; i<len; i++) {
        if(str[i] != '#'){
            buffer[pos] = str[i];
            pos = pos +1;
        }else{
            if(pos != 0) {
                buffer[pos] = '\0';
                if(1 != sscanf(buffer , "%llx" , &(content[num]))){
                    free(content);
                    return NULL;
                }
                num = num + 1;
                pos = 0;
            }
        }
    }
    content[num] = '\0';
    num++;
    content = realloc(content, num * sizeof(int64));
    return init_signature_raw(content, num-1);
}

/**
 * @brief convert a signed declation to string
 * 
 * @param prc : declaration to convert
 * @return char* 
 */
char* protected_to_str(const Protected* prc){
    assert(prc);
    char* ks = key_to_str(prc->pKey);
    char* ss = signature_to_str(prc->sig);
    int N = strlen(ks) + strlen(prc->msg) + strlen(ss) + 2;
    char* ret = malloc(sizeof(char)*(N+1));
    if(!ret){
        MALLOC_ERROR("coudln't convert protected to string");
        return NULL;
    }
    ret[0] = '\0';
    strcat(ret, ks);
    strcat(ret, " ");
    strcat(ret, prc->msg);
    strcat(ret, " ");
    strcat(ret, ss);
    free(ks);
    free(ss);
    return ret;
}

/**
 * @brief parse a string to a signed declaration
 * 
 * @param str : string to parse
 * @return Protected* 
 */
Protected* str_to_protected(const char* str){
    return str_to_protected_len(str, strlen(str));
}

/**
 * @brief parse a string to a signed declaration
 * 
 * @param str : string to parse
 * @param slen : length of the string
 * @return Protected* 
 */
Protected* str_to_protected_len(const char* str, size_t slen){
    Key* k = str_to_key(str);
    if(!k) return NULL;
    size_t msg_s = 0;
    // we know ")" exists because key was parsed successfully
    while(str[msg_s] != ')') msg_s++;
    msg_s += 2;

    const size_t len = slen;
    size_t msg_e = len-1;
    // there are no spaces in the signature
    while(str[msg_e] != ' ') msg_e--;
    msg_e -= 1;

    if(msg_e < msg_s){
        free(k);
        return NULL;
    }

    // Signature* sig = str_to_signature(str+msg_e+2);
    Signature* sig = str_to_signature_len(str+msg_e+2, slen-msg_e-2);
    if(!sig) {
        free(k);
        return NULL;
    }

    // we block out the message from the beginning and the end, 
    // thus the contents of message cannot break our program.
    char* msg = strndup(str+msg_s, msg_e-msg_s+1);

    return init_protected_raw(k, msg, sig);
}