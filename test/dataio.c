#include "lib/dataio.h"
#include "test/test.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

int main(){
    srand(time(NULL));
    char* dir = "../temp";
    generate_random_data(100,10,dir);
    // const char* tempdir = "../temp";
    // generate_random_data(50, 10, tempdir);

    // char declarations[500];
    // strcpy(declarations, tempdir);
    // strcat(declarations, "/declarations.txt");
    // CellProtected* plist = read_protected(declarations);
    // print_protected_list(plist);

    // free_list_protected(plist);

    CellKey* cellkey = read_public_keys("../temp/keys.txt");
    print_list_keys(cellkey);
    free_list_keys(cellkey);
    TEST_SUMMARY();
}