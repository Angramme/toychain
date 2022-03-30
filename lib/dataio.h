#ifndef _DATA_IO_H
#define _DATA_IO_H

#include "lib/rsa.h"

typedef struct cellKey{
    Key* data;
    struct cellKey* next;
}CellKey;
#include "lib/sign.h"

typedef struct cellProtected {
    Protected* data;
    struct cellProtected* next;
} CellProtected;

CellProtected* create_cell_protected(Protected* pr);
void prepend_protected(CellProtected** list, Protected* pr);
CellProtected* read_protected(const char* filename);
void print_protected_list(CellProtected* list);
CellProtected* copy_protected_list(const CellProtected* o);
void free_cell_protected(CellProtected* c);
void free_list_protected(CellProtected* c);
void remove_fraudulent_blocks(CellProtected** list);

void generate_random_data(int nv, int nc, const char* dir);
CellKey* create_cell_key(Key* key);
CellKey* insert_cell_key(CellKey* cellkey, Key* key);
CellKey* read_public_keys(char* file);
void print_list_keys(CellKey* LCK);
void free_cell_keys(CellKey* c); //delete
void free_list_keys(CellKey* cellkey);
#endif