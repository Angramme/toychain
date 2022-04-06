#ifndef _BLOCKCHAIN_H
#define _BLOCKCHAIN_H

#include "lib/rsa.h"
#include "lib/dataio.h"

typedef struct block {
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
}Block;

void write_block(char* filename, Block* b);
Block* read_block(char* filename);
#endif