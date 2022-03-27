#include "lib/dataio.h"
#include "lib/rsa.h"
#include "lib/sign.h"
#include "lib/error.h"
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
void generate_random_data(int nv, int nc, const char* dir){
    assert(strlen(dir)+20 < 500);
    char pth[500];
    strcpy(pth, dir);
    strcat(pth, "/keys.txt");
    FILE* keys = fopen(pth, "w+");
    if(!keys) printf("couldn't open %s !\n", pth);
    strcpy(pth, dir);
    strcat(pth, "/candidates.txt");
    FILE* cand = fopen(pth, "w+");
    if(!cand) printf("couldn't open %s !\n", pth);
    strcpy(pth, dir);
    strcat(pth, "/declarations.txt");
    FILE* decl = fopen(pth, "w+");
    if(!decl) printf("couldn't open %s !\n", pth);
    if(!keys || !cand || !decl) return;

    Key* tab_pkey = malloc(sizeof(Key) * nv);
    Key* tab_skey = malloc(sizeof(Key) * nv);
    Key* pkey_cand = malloc(sizeof(Key) * nc);
    Key* skey_cand = malloc(sizeof(Key) * nc);
    if(!tab_pkey || !tab_skey || !pkey_cand || !skey_cand){
        MALLOC_ERROR("coudln't malloc!!!");
        return;
    }

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
        Protected* p = init_protected_raw(&tab_pkey[i], msg, s);
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
 * !!! takes ownership of key!
 * 
 * @param key key to assign
 * @return CellKey* 
 */
CellKey* create_cell_key_raw(Key* key){
    CellKey* newcell = malloc(sizeof(CellKey));
    if(!newcell){
        MALLOC_ERROR("coudln't create cell key");
        return NULL;
    }
    newcell->data = key;
    newcell->next = NULL;
    return newcell;
}

/**
 * @brief Allocate and initialize a CellKey object.
 * 
 * @param key key to assign
 * @return CellKey* 
 */
CellKey* create_cell_key(Key* key){
    return create_cell_key_raw(copy_key(key));
}

/**
 * @brief Insert a new element to cellkey (head insertion).
 * 
 * @param cellkey list of key
 * @param key key to insert
 * @return CellKey* 
 */
CellKey* insert_cell_key(CellKey* cellkey, Key* key){
    CellKey* newck = create_cell_key_raw(key);
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
    if(!res){
        MALLOC_ERROR("couldn't read public key");   
        return NULL;
    }
    res->data = NULL;

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
 * @brief Create a cell protected object
 * !!! takes ownership of protected
 * 
 * @param pr 
 * @return CellProtected* 
 */
CellProtected* create_cell_protected_raw(Protected* pr){
    CellProtected* ret = malloc(sizeof(CellProtected));
    if(!ret){
        MALLOC_ERROR("coudln't create cell protected")
        return NULL;
    }
    ret->data = pr;
    ret->next = NULL;
    return ret;
}

/**
 * @brief Create a cell protected object
 * 
 * @param pr 
 * @return CellProtected* 
 */
CellProtected* create_cell_protected(Protected* pr){
    return create_cell_protected_raw(copy_protected(pr));
}

/**
 * @brief Prepend CellProtected to a list
 * 
 * @param list 
 * @param pr 
 */
void prepend_protected(CellProtected** list, Protected* pr){
    CellProtected* old = *list;
    *list = create_cell_protected_raw(pr);
    (*list)->next = old;
}

/**
 * @brief read protected list from file.
 * data format is lines in 
 * protected_to_str format each 
 * representing a protected
 * 
 * @param filename 
 * @return CellProtected* 
 */
CellProtected* read_protected(const char* filename){
    FILE* file = fopen(filename, "r");
    if(!file){
        printf("couldn't open %s !\n", filename);
        return NULL;
    }
    size_t buff_size = 500;
    char* buffer = malloc(sizeof(char)*buff_size);
    if(!buffer){
        MALLOC_ERROR("couldn't read prtected");
        return NULL;
    }

    CellProtected* ret = NULL;

    while(getline(&buffer, &buff_size, file) > 1){
        Protected* proc = str_to_protected(buffer);
        prepend_protected(&ret, proc);
    }

    free(buffer);
    fclose(file);
    return ret;
}

/**
 * @brief prints the chained list of protected
 * 
 * @param list 
 */
void print_protected_list(CellProtected* list){
    while(list){
        char* str = protected_to_str(list->data);
        printf("%s \n", str);
        free(str);
        list = list->next;
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

/**
 * @brief correspond au delete_cell_protected dans le sujet
 * 
 * @param c 
 */
void free_cell_protected(CellProtected* c){
    free_protected(c->data);
    free(c);
}

/**
 * @brief frees the list of pretected
 * 
 * @param c 
 */
void free_list_protected(CellProtected* c){
    while(c){
        CellProtected* next = c->next;
        free_cell_protected(c);
        c = next;
    }
}

/**
 * @brief removes fraudulent block from the chain. Fraudulent blocks are the ones that don't verify the signature.
 * 
 * @param list 
 */
void remove_fraudulent_blocks(CellProtected** list){
    while(*list){
        if(!verify((*list)->data)){
            CellProtected* tmp = *list;
            *list = (*list)->next;
            free(tmp);
        }
        list = &((*list)->next);
    }
}
