#ifndef _BLOCKCHAIN_H
#define _BLOCKCHAIN_H

#include <openssl/sha.h>
#include "lib/types.h"
#include "lib/rsa.h"
#include "lib/dataio.h"

#define BLOCK_HASH_SIZE SHA256_DIGEST_LENGTH

typedef struct block {
    Key* author;
    CellProtected* votes;
    uint8* hash;
    uint8* previous_hash;
    int nonce;
}Block;

bool write_block(const char* filename, const Block* b);
Block* read_block(const char* filename);
char* block_head_to_str(const Block* b);
void block_nonce_to_str(char* headstr, const Block* b);
char* block_to_str(const Block* b);
uint8* hash_string(const char* s);
void compute_proof_of_work(Block *B, int d);
void print_sha256_hash(const uint8* hash);
bool compare_hash(const uint8* A, const uint8* B);
uint8* copy_hash(const uint8* hash);
bool str_to_hash(const char* str, uint8** ret);
char* hash_to_str(const uint8* hash);
bool verify_block(const Block*, int d);
void free_block(Block* b);

Block* init_block_raw(Key* auth, CellProtected* votes, uint8* prev_hash);
Block* init_block(const Key* auth, const CellProtected* votes, const uint8* prev_hash);


#endif