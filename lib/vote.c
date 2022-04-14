#include "lib/vote.h"
#include "lib/types.h"
#include "lib/mmath.h"
#include "lib/dataio.h"
#include "lib/error.h"
#include <stdlib.h>
#include <assert.h>

/**
 * @file vote.c
 * @brief A set of functions for the voting system.
 * 
 */

/**
 * @brief Create a hashcell raw object. Takes ownership of key.
 * @param key 
 * @return HashCell* 
 */
HashCell* create_hashcell_raw(Key* key){
    HashCell* ret = malloc(sizeof(HashCell));
    if(!ret){
        MALLOC_ERROR("HashCell allocation failed");
        return NULL;
    }
    ret->key = key;
    ret->val = 0;
    return ret;
}

/**
 * @brief Create a hashcell object. Copies key.
 * @param key 
 * @return HashCell* 
 */
HashCell* create_hashcell(const Key* key){
    return create_hashcell_raw(copy_key(key));
}

/**
 * @brief Free a HasCell object.
 * @param hc Object to free.
 */
void free_hashcell(HashCell* hc){
    free(hc->key);
    free(hc);
}

/**
 * @brief Hash function for Key arrays (HashTable).
 * 
 * @param key 
 * @param size length of the array
 * @return uint32 : position in the array
 */
uint32 hash_function(const Key* key, int size){
    const int64 tab[] = {key->n, key->v};
    const uint32 hs = jenkins_one_at_a_time_hash((uint8*)&tab, sizeof(tab)/sizeof(uint8));
    const double X = GOLDEN_RATIO * (double)hs;
    return (uint32)(size * (X - (double)(uint32)(X)));
}

/**
 * @brief Find the position to place k at in the HasTable t.
 * If the position in the array is taken, find a new one using linear probing.
 * If no position was found, returns the originally found position of hash_function().
 * @param t Table to store in
 * @param k Key to store
 * @return uint32 : found position.
 */
uint32 find_position(const HashTable* t, const Key* k){
    assert(t && t->tab);
    const uint32 Io = hash_function(k, t->size);
    uint32 i = Io % t->size;
    while(true){
        if(t->tab[i] == NULL) return i;
        const Key* k2 = t->tab[i]->key;
        if(k2->n == k->n && k2->v == k->v) return i;

        i++;
        i %= t->size;
        if(i == Io) return Io;
    }
}

/**
 * @brief Create a HashTable object containing keys elements.
 * Keys position are found with find_position()..
 * 
 * @param keys keys to store
 * @param size size of the table
 * @return HashTable* 
 */
HashTable* create_hashtable(const CellKey* keys, size_t size){
    HashTable* T = malloc(sizeof(HashTable));
    if(!T){
        MALLOC_ERROR("T failed allocation");
        return NULL;
    }

    T->size = size;
    T->tab = malloc(sizeof(HashCell*)*size);
    if(!T->tab){
        MALLOC_ERROR("T->tab failed allocation");
        free(T);
        return NULL;
    }
    size_t i;
    for(i=0; i<size; i++) T->tab[i] = NULL;
    
    size_t counter = 0;
    while(keys){
        assert(counter <= size);
        counter++;
        uint32 i = find_position(T, keys->data);
        T->tab[i] = create_hashcell(keys->data);
        keys = keys->next;
    }

    return T;
}

/**
 * @brief Free a HashTable object and all the elements of the table.
 * (Correspond to delete_hashtable in the project subject).
 * 
 * @param t object to free
 */
void free_hashtable(HashTable* t){
    if(!t)return;
    size_t i;
    for(i=0; i<t->size; i++){
        if(t->tab[i]) free_hashcell(t->tab[i]);
    }
    free(t->tab);
    free(t);
}

/**
 * @brief Simulates an election process and returns the winner.
 * Checks if declarations are valid and update vote numbers for each candidate.
 * @param decl list of declaration
 * @param cadidates list of candidates
 * @param voters list of voters
 * @param sizeC size of candidates
 * @param sizeV size of voters
 * @return Key* copy of the key of the winning candidate
 */
Key* compute_winner(const CellProtected* decl, const CellKey* cadidates, const CellKey* voters, int sizeC, int sizeV){
    HashTable* Hc = create_hashtable(cadidates, sizeC+1);
    HashTable* Hv = create_hashtable(voters, sizeV+1);
    if(!Hc || !Hv)return NULL;

    while(decl){
        // make sure the vote is legitimate
        if(!verify(decl->data)){
            decl = decl->next;    
            continue;
        }

        // make sure the person hasn't voted already.
        uint32 iV = find_position(Hv, decl->data->pKey);
        if(Hv->tab[iV]->val != 0){
            decl = decl->next;
            continue;
        }

        // make sure the target is a candidate.
        Key* cand_key = str_to_key(decl->data->msg);
        if(!cand_key){
            fprintf(stderr, "cand_key is NULL\n");
            free_hashtable(Hc);
            free_hashtable(Hv);
            return NULL;
        }
        uint32 iC = find_position(Hc, cand_key);
        if(Hc->tab[iC] == NULL || Hc->tab[iC]->key->n != cand_key->n || Hc->tab[iC]->key->v != cand_key->v){
            free(cand_key);
            decl = decl->next;
            continue;
        }
        free(cand_key);

        // update counter
        Hc->tab[iC]->val++;
        Hv->tab[iV]->val = 1;

        decl = decl->next;
    }

    size_t imax = 0;
    while(!Hc->tab[imax] && imax < Hc->size) imax++;
    size_t i;
    for(i = 0; i < Hc->size; i++){
        if(Hc->tab[i] && Hc->tab[imax]->val < Hc->tab[i]->val) imax = i;
    }

    Key* ret = copy_key(Hc->tab[imax]->key);

    free_hashtable(Hc);
    free_hashtable(Hv);

    return ret;
}