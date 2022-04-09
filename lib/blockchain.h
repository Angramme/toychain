#ifndef _BLOCKCHAIN_H
#define _BLOCKCHAIN_H

#include "lib/rsa.h"
#include "lib/dataio.h"

// TODO: change to the actual constant after openssl installation
// #define BLOCK_HASH_SIZE SHA256_DIGEST_LENGTH
#define BLOCK_HASH_SIZE 10

typedef struct block {
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
}Block;

void write_block(char* filename, Block* b);
Block* read_block(char* filename);
char* block_to_str(const Block* b);


#endif