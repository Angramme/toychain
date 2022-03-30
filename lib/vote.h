#ifndef _VOTE_H_
#define _VOTE_H_

#include "lib/sign.h"

typedef struct hashcell {
    Key* key;
    int val;
} HashCell;

typedef struct hashtable {
    HashCell** tab;
    int size;
} HashTable;

#endif