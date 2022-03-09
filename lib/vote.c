#include "lib/vote.h"
#include "lib/rsa.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    int64* con = encrypt(mess, sKey->v, sKey->n);
    int len = strlen(mess);
    return init_signature(con, len);
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
        sprintf(buffer, "%llx " , sgn->xs[i]);
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
                sscanf(buffer , "%llx" , &(content[num]));
                num = num + 1;
                pos = 0;
            }
        }
    }
    content[num] = '\0';
    num++;
    content = realloc(content, num * sizeof(int64));
    return init_signature(content, num);
}