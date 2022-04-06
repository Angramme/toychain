#include "blockchain.h"
#include <stdio.h>

void ecrire_block(char* filename, Block* b){
    FILE* f = fopen(filename, "w");
    if(!f){
        fprintf(stderr, "ecrire_block : failed opening\n");
        return;
    }
    if(!b){
        fprintf(stderr, "ecrire_block : null argument\n");
        return;
    }

    fprintf(f,"%s\n", key_to_str(b->author));
    fprintf(f,"%s\n", b->hash);
    fprintf(f, "%s\n", b->previous_hash);
    fprintf(f, "%d\n", b->nonce);
    //fprintf(f, "%s", list_protected_to_str(b->votes));

    fclose(f);
}
