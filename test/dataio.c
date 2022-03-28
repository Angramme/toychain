#include "lib/dataio.h"
#include "test/test.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

int main(){
    srand(time(NULL));
    char* dir = "../temp";
    char path[500];
    generate_random_data(30,10,dir);

    strcpy(path, dir);
    strcat(path, "/declarations.txt");
    CellProtected* plist = read_protected(path);
    print_protected_list(plist);
    free_list_protected(plist);

    strcpy(path, dir);
    strcat(path, "/keys.txt");
    CellKey* cellkey = read_public_keys(path);
    print_list_keys(cellkey);
    free_list_keys(cellkey);
    
    TEST_SUMMARY();
}