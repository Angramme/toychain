#include "lib/vote.h"
#include "lib/types.h"
#include "lib/mmath.h"
#include "lib/dataio.h"
#include <stdlib.h>
#include <assert.h>

/**
 * @file vote.c
 * @brief A set of functions for the voting system.
 * 
 */

HashCell* create_hashcell_raw(Key* key){
    HashCell* ret = malloc(sizeof(HashCell));
    ret->key = key;
    ret->val = 0;
    return ret;
}

HashCell* create_hashcell(const Key* key){
    return create_hashcell_raw(copy_key(key));
}

void free_hashcell(HashCell* hc){
    free(hc->key);
    free(hc);
}

uint32 hash_function(const Key* key, int size){
    const int64 tab[] = {key->n, key->v};
    const uint32 hs = jenkins_one_at_a_time_hash((uint8*)&tab, sizeof(tab)/sizeof(uint8));
    const double X = GOLDEN_RATIO * (double)hs;
    return (uint32)(size * (X - (double)(uint32)(X)));
}

uint32 find_position(const HashTable* t, const Key* k){
    const uint32 Io = hash_function(k, t->size);
    uint32 i = Io;
    while(true){
        const Key* k2 = t->tab[i]->key;
        if(k2->n == k->n && k2->v == k->v) return i;
        if(t->tab[i] == NULL) return i;

        i++;
        i %= t->size;
        if(i == Io) return Io;
    }
}

HashTable* create_hashtable(const CellKey* keys, size_t size){
    HashTable* T = malloc(sizeof(HashTable));
    T->size = size;
    T->tab = malloc(sizeof(HashCell*)*size);
    
    size_t counter = 0;
    while(keys){
        uint32 i = find_position(T, keys->data);
        T->tab[i] = create_hashcell(keys->data);
        keys = keys->next;
        assert(counter++ <= size);
    }

    return T;
}

/**
 * @brief correspond a delete_hashtable
 * 
 * @param t 
 */
void free_hashtable(HashTable* t){
    for(size_t i=0; i<t->size; i++){
        free_hashcell(t->tab[i]);
    }
    free(t->tab);
    free(t);
}

/**
 * @brief 
 * 
 * @param decl 
 * @param cadidates 
 * @param voters 
 * @param sizeC 
 * @param sizeV 
 * @return Key* copy of the key of the winning candidate
 */
Key* compute_winner(const CellProtected* decl, const CellKey* cadidates, const CellKey* voters, int sizeC, int sizeV){
    HashTable* Hc = create_hashtable(cadidates, sizeC);
    HashTable* Hv = create_hashtable(voters, sizeV);

    while(decl){
        // make sure the vote is legitimate
        if(!verify(decl->data)) continue;

        // make sure the person hasn't voted already.
        uint32 iV = find_position(Hv, decl->data->pKey);
        if(Hv->tab[iV]->val != 0) continue;

        // make sure the target is a candidate.
        Key* cand_key = str_to_key(decl->data->msg);
        uint32 iC = find_position(Hc, cand_key);
        if(Hc->tab[iC] == NULL || Hc->tab[iC]->key->n != cand_key->n || Hc->tab[iC]->key->v != cand_key->v){
            free(cand_key);
            continue;
        }
        free(cand_key);

        // update counter
        Hc->tab[iC]->val++;
        Hv->tab[iV]->val = 1;

        decl = decl->next;
    }

    size_t imax = 0;
    for(size_t i = 0; i < Hc->size; i++){
        if(Hc->tab[imax]->val < Hc->tab[i]->val) imax = i;
    }

    free_hashtable(Hc);
    free_hashtable(Hv);

    return copy_key(Hc->tab[imax]->key);
}