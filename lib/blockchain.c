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
    char* l_str = list_protected_to_str(b->votes);

    char* chash = hash_to_str(b->hash);
    char* cphash = hash_to_str(b->previous_hash);

    fprintf(f,"%s\n", keystr);
    fprintf(f,"%s\n", chash);
    fprintf(f, "%s\n", cphash);
    fprintf(f, "%d\n", b->nonce);
    fprintf(f, "%s\n", l_str);

    free(l_str);
    free(keystr);
    free(chash);
    free(cphash);

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
    fseek(f, 0L, SEEK_END);
    numbytes = ftell(f);
    fseek(f, 0L, SEEK_SET);

    buffer = (char*)calloc(numbytes, sizeof(char));
    if(buffer == NULL){
        MALLOC_ERROR("calloc failed");
        return NULL;
    }
    fread(buffer, sizeof(char), numbytes, f);
    fclose(f);

    int l = 0;
    int start;
    int end;
    for(int t = 0; t<4; t++){
        while(buffer[l] == '\n')l++;
        start = l;
        while(buffer[l] != '\n'){
            l++;
        }
        end = l;
        char* temp = malloc(sizeof(char)*(end-start+1));
        int ind = 0;
        while(ind < end-start){
            temp[ind] = buffer[start + ind];
            ind++;
        }
        temp[ind+1] = '\0';
        printf("%s\n", temp);

        if(t == 0){
            Key* nkey = str_to_key(temp);
            newb->author = nkey;
        }
        if(t == 1){
            uint8* hash = str_to_hash(temp);
            newb->hash = hash;
        }
        if(t == 2){
            uint8* p_hash = str_to_hash(temp);
            newb->previous_hash = p_hash;
        }
        if(t==3){
            int n = atoi(temp);
            newb->nonce=n;
        }
        free(temp);
        while(buffer[l] == '\n')l++;
    }
    CellProtected* votes = str_to_list_protected(buffer + l);
    newb->votes = votes;
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
    ret[maxsize-1] = '\0';
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
    char* kstr = key_to_str(b->author);
    char* lstr = list_protected_to_str(b->votes);
    size_t len = strlen(kstr) + BLOCK_HASH_SIZE*2 + strlen(lstr) + 10 + 1;
    char* ret = malloc(sizeof(char)*len);
    ret[0] = '\0';
    strcat(ret, kstr);
    free(kstr);
    char* hshs = hash_to_str(b->previous_hash);
    strcat(ret, hshs);
    free(hshs);
    strcat(ret, lstr);
    free(lstr);
    
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
 * @brief hashes string s using sha256. Even tho the result is a pointer, you don't have to free it!
 * OpenSSL magic
 * 
 * @param s 
 * @return uint8* 
 */
uint8* hash_string(const char* s){
    return SHA256((const unsigned char*)s, strlen(s), 0) ;
}

/**
 * @brief checks if the number of zeros in hash (as per the hexadecimal representation) is at least d
 * 
 * @param hash 
 * @param d 
 * @return true 
 * @return false 
 */
bool check_zeros(const uint8* hash, size_t d){
    if(!hash) return false;
    size_t n = d/2;
    n = n < BLOCK_HASH_SIZE ? n : BLOCK_HASH_SIZE;
    for(size_t i=0; i<n; i++)
        if(hash[i] != 0) return false;
    if(d%2 == 1 && hash[n] & 0b11110000) return false;
    return true;
}

/**
 * @brief compute proof of work of block B with d zeros in hexadecimal representation
 * 
 * @param B 
 * @param d 
 */
void compute_proof_of_work(Block *B, int d){
    char* bstr = block_to_str(B);
    while(!check_zeros(B->hash, d)){
        B->nonce++;
        // modify the nonce in the string.
        // makes the program faster than reconverting 
        // the whole block each time...
        block_nonce_to_str(bstr, B); 
        uint8* hsh = hash_string(bstr);
        if(B->hash) free(B->hash);
        B->hash = copy_hash(hsh);
    }
    free(bstr);
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
    if(!A && !B) return true;
    if(!A || !B) return false;
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
    char* str = block_to_str(B);
    const uint8* hsh = hash_string(str); // <- we don't have to free this
    free(str);
    return check_zeros(hsh, d) && compare_hash(B->hash, hsh);
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

/**
 * @brief frees block
 * 
 * @param b 
 */
void free_block(Block* b){
    free(b->author);
    free(b->hash);
    free(b->previous_hash);
    free_list_protected(b->votes);
    free(b);
}