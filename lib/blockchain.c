#include "lib/blockchain.h"
#include "lib/dataio.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/error.h"
#include <string.h>

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

/**
 * @brief convert sha256 hash to string representation
 * 
 * @param hash 
 * @return char* 
 */
char* hash_to_str(const uint8* hash){
    const size_t maxsize = 2*BLOCK_HASH_SIZE +1;
    char* ret = malloc(sizeof(char)*maxsize);
    ret[0] = '\0';
    for(size_t i=0; i<BLOCK_HASH_SIZE; i++){
        char buf[4];
        sprintf(buf, "%02x", hash[i]); 
        ret[i*2] = buf[0];
        ret[i*2+1] = buf[1];
    }
    return ret;
}

/**
 * @brief convert string sha256 hash representation to sha256 hash
 * 
 * @param str 
 * @return uint8* 
 */
uint8* str_to_hash(const char* str){
    uint8* ret = malloc(sizeof(uint8)*BLOCK_HASH_SIZE);
    for(size_t i=0; i<BLOCK_HASH_SIZE; i++){
        char buff[3];
        buff[2] = '\0';
        memcpy(buff, str + i*2, 2);
        unsigned int V;
        sscanf(buff, "%02x", &V);
        ret[i] = (uint8)V;
    }
    return ret;
}

/**
 * @brief convert block head to string and leave extra space for nonce to be appended
 * 
 * @param b 
 * @return char* 
 */
char* block_head_to_str(const Block* b){
    const char* kstr = key_to_str(b->author);
    const char* lstr = list_protected_to_str(b->votes);
    size_t len = strlen(kstr) + BLOCK_HASH_SIZE*2 + strlen(lstr) + 10 + 1;
    char* ret = malloc(sizeof(char)*len);
    ret[0] = '\0';
    strcat(ret, kstr);
    char* hshs = hash_to_str(b->previous_hash);
    strcat(ret, hshs);
    free(hshs);
    strcat(ret, lstr);
    
    return ret;
}

/**
 * @brief append nonce to the block head string. 
 * If nonce is already appended, the function will 
 * delete the old nonce and append the new one.
 * 
 * @param headstr 
 * @param b 
 */
void block_nonce_to_str(char* headstr, const Block* b){
    const size_t l = strlen(headstr);
    char buf[12];
    sprintf(buf, "|%08x|", b->nonce);
    if(headstr[l-1] == '|'){
        // delete the old nonce first
        size_t beg = l-2;
        while(headstr[beg] != '|') beg--;
        headstr[beg] = '\0';
    }
    strcat(headstr, buf);
}

/**
 * @brief convert block to string
 * 
 * @param b 
 * @return char* 
 */
char* block_to_str(const Block* b){
    char* ret = block_head_to_str(b);
    block_nonce_to_str(ret, b);
    return ret;
}

/**
 * @brief hashes string s using sha256
 * 
 * @param s 
 * @return uint8* 
 */
uint8* hash_string(const char* s){
    return SHA256((const unsigned char*)s, strlen(s), 0) ;
}

/**
 * @brief compute proof of work of block B with d zeros in hexadecimal representation
 * 
 * @param B 
 * @param d 
 */
void compute_proof_of_work(Block *B, int d){
    int cnt = 0;
    char* bstr = block_to_str(B);
    while(cnt < d*4){
        B->nonce++;
        // modify the nonce in the string.
        // makes the program faster than reconverting 
        // the whole block each time...
        block_nonce_to_str(bstr, B); 
        uint8* hsh = hash_string(bstr);
        if(B->hash) free(B->hash);
        B->hash = hsh;
        cnt = 0;
        while (hsh[cnt] == 0) cnt++;
    }
}

/**
 * @brief print sha256 hash
 * 
 * @param hash 
 */
void print_sha256_hash(const uint8* hash){
    for(size_t i = 0; i < BLOCK_HASH_SIZE; i++)
        printf("%02x", hash[i]);
    putchar('\n');
}

/**
 * @brief checks if A and B are the same hash
 * 
 * @param A 
 * @param B 
 * @return true 
 * @return false 
 */
bool compare_hash(const uint8* A, const uint8* B){
    bool same = true;
    for(int i = 0; (i < BLOCK_HASH_SIZE) && same; i++)
        same = same && A[i] == B[i];
    return same;
}

/**
 * @brief copy hash
 * 
 * @param hash 
 * @return uint8* 
 */
uint8* copy_hash(const uint8* hash){
    uint8* ret = malloc(sizeof(uint8)*(BLOCK_HASH_SIZE+1));
    memcpy(ret, hash, BLOCK_HASH_SIZE);
    ret[BLOCK_HASH_SIZE] = '\0';
    return ret;
}

/**
 * @brief verifies if the block is valid with d zeros in hexadecimal representation
 * 
 * @param d 
 * @return true 
 * @return false 
 */
bool verify_block(const Block* B, int d){
    const char* str = block_to_str(B);
    const uint8* hsh = hash_string(str);
    size_t cnt = 0;
    while (hsh[cnt] == 0) cnt++;
    return cnt >= d*4 && compare_hash(B->hash, hsh);
}

/**
 * @brief init a block object. Takes ownership of all auth, votes and prev_hash
 * 
 * @param auth 
 * @param votes 
 * @param prev_hash 
 * @return Block* 
 */
Block* init_block_raw(Key* auth, CellProtected* votes, uint8* prev_hash){
    Block* b = malloc(sizeof(Block));
    b->author = auth;
    b->votes = votes;
    b->hash = NULL;
    b->previous_hash = prev_hash;
    b->nonce = 0;
    return b;
}

/**
 * @brief init block object, copies all parameters.
 * 
 * @param auth 
 * @param votes 
 * @param prev_hash 
 * @return Block* 
 */
Block* init_block(const Key* auth, const CellProtected* votes, const uint8* prev_hash){
    return init_block_raw(copy_key(auth), copy_protected_list(votes), copy_hash(prev_hash));
}