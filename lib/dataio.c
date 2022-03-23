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