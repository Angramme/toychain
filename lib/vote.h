#ifndef _VOTE_H_
#define _VOTE_H_

#include "lib/sign.h"
#include "lib/dataio.h"
#include "lib/types.h"
#include <stddef.h>

/**
 * @file vote.h
 * @brief header of vote.c
 */

/**
 * Structure for storing a single Key.
 */
typedef struct hashcell {
    Key* key;
    int val;
} HashCell;

/**
 * Structure to store Key objects as an array.
 */
typedef struct hashtable {
    HashCell** tab;
    int size;
} HashTable;

HashCell* create_hashcell_raw(Key* key);
HashCell* create_hashcell(const Key* key);
void free_hashcell(HashCell* hc);
uint32 hash_function(const Key* key, int size);
uint32 find_position(const HashTable* t, const Key* k);
HashTable* create_hashtable(const CellKey* keys, size_t size);
void free_hashtable(HashTable* t);
Key* compute_winner(const CellProtected* decl, const CellKey* cadidates, const CellKey* voters, int sizeC, int sizeV);

#endif