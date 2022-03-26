#include "lib/dataio.h"
#include "lib/rsa.h"
#include "lib/sign.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief generates nv public and private keys and prints them in ./test/keys.txt.
 * Chooses nc couple of keys among them and print them in ./test/candidates.txt.
 * Then for all nv public keys, assign a random public key from the previous nc 
 * couples converted using key_to_str(), and its signed version using sign().
 * Prints the results in ./test/declarations.txt.
 * 
 * @param nv number of couple of keys(citizen) to be generated
 * @param nc number of candidates among the nv couples
 */
void generate_random_data(int nv, int nc, char* dir){
    assert(strlen(dir)+20 < 500);
    char pth[500];
    strcpy(pth, dir);
    strcat(pth, "/keys.txt");
    FILE* keys = fopen(pth, "w+");
    strcpy(pth, dir);
    strcat(pth, "/candidates.txt");
    FILE* cand = fopen(pth, "w+");
    strcpy(pth, dir);
    strcat(pth, "/declarations.txt");
    FILE* decl = fopen(pth, "w+");
    if(!keys || !cand || !decl)return;

    Key* tab_pkey = malloc(sizeof(Key) * nv);
    Key* tab_skey = malloc(sizeof(Key) * nv);
    Key* pkey_cand = malloc(sizeof(Key) * nc);
    Key* skey_cand = malloc(sizeof(Key) * nc);

    //generation des nv citoyens
    for(int i = 0; i < nv; i++){
        init_pair_keys(&tab_pkey[i], &tab_skey[i], 8, 12);
        char* pstr = key_to_str(&tab_pkey[i]);
        char* sstr = key_to_str(&tab_skey[i]);
        fprintf(keys, "%s %s\n", pstr, sstr);
        free(pstr);
        free(sstr);
    }

    //generation des nc candidats
    for(int i = 0; i < nc; i++){
        int l = rand()%nv; //possibilité de tomber sur la meme cle, a corriger ou non selon les consignes
        init_key(&pkey_cand[i], tab_pkey[l].v, tab_pkey[l].n);
        init_key(&skey_cand[i], tab_skey[l].v, tab_skey[l].n);
        char* pstr = key_to_str(&tab_pkey[l]);
        fprintf(cand, "%s\n", pstr);
        free(pstr);
    }

    //generation des nv declarations
    for(int i = 0; i < nv; i++){
        Key* rcand = &pkey_cand[rand()%nc];
        char* msg = key_to_str(rcand);

        Signature* s = sign(msg, &tab_pkey[i]);
        Protected* p = init_protected(&tab_pkey[i], msg, s);
        char* protstr = protected_to_str(p);
        fprintf(decl, "%s\n", protstr);

        //j'utilise pas free_signature et free_protected car ils feraient des free en trop dans cette situation
        free(msg);
        free(s->xs);
        free(s);
        free(p);
        free(protstr);
    }
    
    free(tab_pkey);
    free(tab_skey);
    free(pkey_cand);
    free(skey_cand);

    fclose(keys);
    fclose(cand);
    fclose(decl);
}
/**
 * @brief Allocate and initialize a CellKey object.
 * 
 * @param key key to assign
 * @return CellKey* 
 */
CellKey* create_cell_key(Key* key){
    CellKey* newcell = malloc(sizeof(CellKey));
    if(!newcell) return NULL;
    newcell->data = key;
    newcell->next = NULL;
    return newcell;
}

/**
 * @brief Insert a new element to cellkey (head insertion).
 * 
 * @param cellkey list of key
 * @param key key to insert
 * @return CellKey* 
 */
CellKey* insert_cell_key(CellKey* cellkey, Key* key){
    CellKey* newck = create_cell_key(key);
    if(!cellkey) {
        cellkey = newck;
    } else {
        newck->next = cellkey;
        cellkey = newck;
    }
    return cellkey;
}

/**
 * @brief Read the public keys of a file, return them in a CellKey structure.
 * 
 * @param file path to the file
 * @return CellKey* 
 */
CellKey* read_public_keys(char* file){
    FILE *f = fopen(file, "r");
    if(!f) {
        fprintf(stderr, "read_public_keys : file opening error");
        return NULL;
    }
    CellKey* res = malloc(sizeof(CellKey));
    if(!res) return NULL;

    char buf[256];
    while(fgets(buf, 256, f)){
        char tmp[100];
        int i = 0;
        while(buf[i] != ')' && i < 99){
            tmp[i] = buf[i];
            i++;
        }
        tmp[i] = buf[i];
        tmp[i+1] = '\0';

        if(!res->data) {
            res->data = str_to_key(tmp);
            res->next =  NULL;
        } else {
            res = insert_cell_key(res, str_to_key(tmp));
        }
    }
    fclose(f);
    return res;
}

/**
 * @brief print the keys from a list
 * 
 * @param LCK list of keys
 */
void print_list_keys(CellKey* LCK){
    if(!LCK){
        printf("print_list_keys() : Argument NULL\n");
        return;
    }
    CellKey* temp = LCK;
    while(temp != NULL && temp->data != NULL){
        char* keystr = key_to_str(temp->data);
        printf("%s\n", keystr);
        free(keystr);
        temp = temp->next;
    }
}

/**
 * @brief free a CellKey
 * 
 * @param c 
 */
void free_cell_keys(CellKey* c){
    if(!c)return;
    free(c->data);
    free(c);
}

/**
 * @brief free a list of keys
 * 
 * @param cellkey 
 */
void free_list_keys(CellKey* cellkey){
    if(!cellkey) return;
    while(cellkey){
        CellKey* temp = cellkey;
        cellkey = cellkey->next;
        free_cell_keys(temp);
    }
}
