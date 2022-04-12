#include "lib/dataio.h"
#include "lib/rsa.h"
#include "lib/sign.h"
#include "lib/error.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/**
 * @file dataio.c
 * @brief A file responsible for reading and writing vote related data.
 * 
 */

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
    if(nc > nv) printf("Wrong arguments, too much candidats : nc = %d > nv = %d\n",nc,nv);
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

    int *is_cand = malloc(nv * sizeof(int)); // pour eviter de retomber sur le meme canditat. 1 si deja candidat, 0 sinon.
    for(int i=0; i<nv; i++) is_cand[i] = 0;
    //generation des nc candidats
    for(int i = 0; i < nc; i++){
        int l = rand()%nv;
        while(is_cand[l] == 1) l = rand()%nv;
        is_cand[l] = 1;
        init_key(&pkey_cand[i], tab_pkey[l].v, tab_pkey[l].n);
        init_key(&skey_cand[i], tab_skey[l].v, tab_skey[l].n);
        char* pstr = key_to_str(&tab_pkey[l]);
        fprintf(cand, "%s\n", pstr);
        free(pstr);
    }
    free(is_cand);

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
        MALLOC_ERROR("couldn't read public keys");   
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
 * !!! takes ownership of pr
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
        MALLOC_ERROR("couldn't read protected");
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
 * @brief converts list of protected to a string.
 * 
 * @param list 
 * @return char* 
 */
char* list_protected_to_str(const CellProtected* list){
    size_t size = 64;
    char* ret = malloc(size*sizeof(char));
    ret[0] = '\0';
    char slen[50];

    while(list){
        char* pstr = protected_to_str(list->data);
        const size_t len = strlen(pstr);
        list = list->next;
        // adding the size is needed for security reasons.
        // suppose we use a newline spearated list of protected,
        // if a message of a protected contains a newline, we will
        // probably risk a segfault. This is high severity security 
        // risk! Thus we go with this approach which is not disturbed 
        // by the message contents. 
        sprintf(slen, "%zx ", len);
        const size_t slenlen = strlen(slen);
        const size_t cursize = (strlen(ret) + slenlen + len + 2);
        if(cursize >= size){ 
            while(cursize >= size) size *= 2;
            ret = realloc(ret, size); 
            if(!ret){ 
                MALLOC_ERROR("error while reallocating memory in list_protected_to_str"); 
                return NULL; 
            } 
        }
        strcat(ret, slen);
        strcat(ret, pstr);
        strcat(ret, "\n");

        free(pstr);
    }
    const size_t len = strlen(ret);
    ret = realloc(ret, len + 2);
    ret[len] = ']';
    ret[len+1] = '\0';
    return ret;
}

/**
 * @brief converts string to a list of protected
 * 
 * @param str 
 * @return CellProtected* 
 */
CellProtected* str_to_list_protected(const char* str){
    CellProtected* list = NULL;

    while(true){
        while(*str == ' ') str++;
        size_t prot_size;
        sscanf(str, " %zx ", &prot_size);
        while(*str != ' ') str++; // skip the heaxdecimal size
        while(*str == ' ') str++; // skip spaces
        Protected* prot = str_to_protected_len(str, prot_size);
        prepend_protected(&list, prot); // takes ownership of proc
        str += prot_size;
        while(*str != '\n') str++; // skip until newline
        while(*str == '\n') str++; // skip all newlines
        if(*str == ']') break;
    }

    { // reverse it 
        CellProtected* prev = NULL, *cur=list, *tmp;
        while(cur){
            tmp = cur->next;
            cur->next = prev;
            prev = cur;
            cur = tmp;
        }
        list = prev;
    }

    return list;
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
 * @brief copies the list of protected values. The copies order is inverted.
 * 
 * @param o 
 * @return CellProtected* 
 */
CellProtected* copy_protected_list(const CellProtected* o){
    CellProtected* ret = NULL;
    while(o){
        prepend_protected(&ret, copy_protected(o->data));
        o = o->next;
    }
    return ret;
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

/**
 * @brief generate random list of protected
 * 
 * @param len 
 * @return CellProtected* 
 */
CellProtected* rand_list_protected(size_t len){
    return rand_list_protected_range(len, 0, CHAR_MAX);
}

/**
 * @brief generate random list of protected with message characters in range low hi
 * 
 * @param len 
 * @param low 
 * @param hi 
 * @return CellProtected* 
 */
CellProtected* rand_list_protected_range(size_t len, char low, char hi){
    CellProtected* randlist = NULL;
    char msg[500];
    for(int i=0; i<len; i++){
        Key pk, sk;
        init_pair_keys(&pk, &sk, 8, 12);
        size_t msgl = rand()%10 + 5;
        assert(msgl < sizeof(msg)/sizeof(char));
        msg[msgl] = '\0';
        for(int j=0; j<msgl; j++){
            do{ msg[j] = rand_int64(low, hi); }
            while(msg[j] == '\0');
        } 
        Signature* sig = sign(msg, &sk);
        Protected* proc = init_protected(&pk, msg, sig);
        free_signature(sig);
        prepend_protected(&randlist, proc); // takes ownership of proc
    }
    return randlist;
}
