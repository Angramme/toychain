#ifndef _DATA_IO_H
#define _DATA_IO_H

#include "lib/rsa.h"

typedef struct cellKey{
    Key* data;
    struct cellKey* next;
}CellKey;

void generate_random_data(int nv, int nc, char* dir);
CellKey* create_cell_key(Key* key);
CellKey* insert_cell_key(CellKey* cellkey, Key* key);
CellKey* read_public_keys(char* file);
void print_list_keys(CellKey* LCK);
void free_cell_keys(CellKey* c); //delete
void free_list_keys(CellKey* cellkey);
#endif