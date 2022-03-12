#include "lib/vote.h"
#include "lib/rsa.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**
 * @file vote.c
 * @brief a collection of functions related to signing messages and the voting system
 * 
 */

/**
* @brief initialize a signaure
*
* @param content : content of the signature
* @param size : size of content
*
* @return Signature* : a new signature on the heap
*/
Signature* init_signature(int64* content, int size){
    Signature* ret = malloc(sizeof(Signature));
    ret->xs = content;
    ret->len = size;
    return ret;
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
    return init_signature(con, len);
}


Protected* init_protected(Key* pKey, char* mess, Signature* sgn){
    assert(pKey && mess && sgn);
    Protected* ret = malloc(sizeof(Protected));
    ret->msg = mess;
    ret->pKey = pKey;
    ret->sig = sgn;
    return ret;
}

void free_protected(Protected* pr){
    free(pr->msg);
    free(pr->pKey);
    free_signature(pr->sig);
    free(pr);
}

/**
* @brief check messages signature
*
* @param Protected* : protected message
*
* @return bool
*/
bool verify(Protected* pr){
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
* @param sgn 
*
* @return char* 
*/
char* signature_to_str(Signature* sgn){
    char* result = malloc (10* sgn->len * sizeof(char));
    result [0]= '#';
    int pos = 1;
    char buffer [156];
    for(int i=0; i < sgn->len; i++) {
        sprintf(buffer, "%llx" , sgn->xs[i]);
        for(int j=0; j<strlen(buffer); j++) {
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
Signature* str_to_signature(char* str){
    int len = strlen(str);
    int64* content = (int64*)malloc(sizeof(int64) * (len+1));
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
    return init_signature(content, num-1);
}

char* protected_to_str(Protected* prc){
    assert(prc);
    char* ks = key_to_str(prc->pKey);
    char* ss = signature_to_str(prc->sig);
    int N = strlen(ks) + strlen(prc->msg) + strlen(ss) + 2;
    char* ret = malloc(sizeof(char)*(N+1));
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

Protected* str_to_protected(char* str){
    Key* k = str_to_key(str);
    if(!k) return NULL;
    int msg_s = 0;
    // we know ")" exists because key was parsed successfully
    while(str[msg_s] != ')') msg_s++;
    msg_s += 2;

    const int len = strlen(str);
    int msg_e = len-1;
    // there are no spaces in the signature
    while(str[msg_e] != ' ') msg_e--;
    msg_e -= 1;

    if(msg_e < msg_s){
        free(k);
        return NULL;
    }

    Signature* sig = str_to_signature(str+msg_e+2);
    if(!sig) {
        free(k);
        return NULL;
    }

    char* msg = strndup(str+msg_s, msg_e-msg_s+1);

    return init_protected(k, msg, sig);
}