#include "lib/blockchain.h"
#include "lib/dataio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/error.h"

/**
 * @brief print the content of a Block struct in a file
 * 
 * @param filename name of the file
 * @param b object to print
 */
void write_block(char* filename, Block* b){
    FILE* f = fopen(filename, "w");
    if(!f){
        fprintf(stderr, "ecrire_block : failed opening\n");
        return;
    }
    if(!b){
        fprintf(stderr, "ecrire_block : null argument\n");
        return;
    }

    char* keystr = key_to_str(b->author);

    fprintf(f,"%s\n", keystr);
    fprintf(f,"%s\n", b->hash);
    fprintf(f, "%s\n", b->previous_hash);
    fprintf(f, "%d\n", b->nonce);
    fprintf(f, "%s\n", list_protected_to_str(b->votes));

    free(keystr);

    fclose(f);
}

/**
 * @brief read a file and save it's content in a Block object
 * 
 * @param filename 
 * @return Block* 
 */
Block* read_block(char* filename){
    FILE* f = fopen(filename, "r");
    if(!f){
        fprintf(stderr, "read_block : failed opening\n");
        return NULL;
    }

    Block* newb = malloc(sizeof(Block));
    if(!newb){
        MALLOC_ERROR("newb");
        return NULL;
    }

    char* buffer;
    long numbytes;

    //to get size of the file
    fseek(f, 0L, SEEK_END);
    numbytes = ftell(f);
    fseek(f, 0L, SEEK_SET);

    buffer = (char*)calloc(numbytes, sizeof(char));
    if(buffer == NULL){
        MALLOC_ERROR("calloc failed");
        return NULL;
    }
    //read the entire file
    fread(buffer, sizeof(char), numbytes, f);
    fclose(f);

    //look for author key
    char strkey[256];
    int i = 0;
    while(buffer[i] != ')' && i < 254){
        strkey[i] = buffer[i];
        i++;
    }
    strkey[i] = buffer[i];
    strkey[i+1] = '\0';
    Key* author = str_to_key(strkey);
    newb->author = author;

    //look for hash, previous_hash and nonce

    //skip remaining ' '
    while(buffer[i] != '\n')i++;

    //look for hash
    int deb = i++;
    while(buffer[i] != ' ' || buffer[i] != '\n')i++;
    int fin = i;

    char hash[fin - deb + 1];
    int ind = 0;
    while(deb < fin)hash[ind++] = buffer[deb++];
    hash[fin] = '\0';
    newb->hash = strdup(hash);

    //skip remaining ' '
    while(buffer[i] != '\n')i++;

    //look for phash
    deb = i++;
    while(buffer[i] != ' ')i++;
    fin = i;
    char phash[fin - deb + 1];
    ind = 0;
    while(deb < fin)phash[ind++] = buffer[deb++];
    phash[fin] = '\0';
    newb->previous_hash = strdup(phash);

    //skip remaining ' '
    while(buffer[i] != '\n')i++;

    //look for nonce
    deb = i++;
    while(buffer[i] != ' ')i++;
    fin = i;
    char nonce[fin - deb + 1];
    ind=0;
    while(deb < fin)nonce[ind++] = buffer[deb++];
    nonce[fin] = '\0';
    int n = atoi(nonce);
    newb->nonce = n;

    //skip remaining ' '
    while(buffer[i] != '\n')i++;
    i++;
    
    //look for declarations
    CellProtected* votes = str_to_list_protected(buffer+i);
    newb->votes = votes;

    free(buffer);
    return newb;
}

char* block_to_str(const Block* b){
    const char* kstr = key_to_str(b->author);
    const char* lstr = list_protected_to_str(b->votes);
    size_t len = strlen(kstr) + BLOCK_HASH_SIZE*2 + strlen(lstr) + 8 + 1;
    char* ret = malloc(sizeof(char)*len);
    ret[0] = '\0';
    strcat(ret, kstr);
    size_t clen = strlen(ret);
    for(size_t i=0; i<BLOCK_HASH_SIZE; i++){
        char buf[4];
        sprintf(buf, "%02x", b->previous_hash[i]); 
        int j=0;
        while(true){
            ret[clen++] = buf[j];
            if(buf[j] == '\0') break;
            j++;
        }
    }
    strcat(ret, lstr);
    {
        char buf[10];
        sprintf(buf, "%08x", b->nonce);
        strcat(ret, buf);
    }
    return ret;
}