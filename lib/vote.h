#ifndef __VOTE_H
#define __VOTE_H

#include "lib/rsa.h"
#include "lib/types.h"
#include <stdbool.h>


/**
 * @file rsa.h
 * @brief header of rsa.c
 */

/**
 * A struct for storing a signature
 */
typedef struct _Signature{
    int64* xs; /**< int64* the value of the signature on the heap */
    int len; /**< int the size of the contents ("\n" at the end is not included in the size) */
} Signature;

Signature* init_signature(int64* content, int size);
void free_signature(Signature* sig);
Signature* sign(char* mess, Key* sKey);

char* signature_to_str(Signature* sgn);
Signature* str_to_signature(char* str);

typedef struct _Protected{
    Key* pKey;
    char* msg;
    Signature* sig;
} Protected;

Protected* init_protected(Key* pKey, char* mess, Signature* sgn);
bool verify(Protected* pr);
char* protected_to_str(Protected* prc);
Protected* str_to_protected(char* str);

#endif