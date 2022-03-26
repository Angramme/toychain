#include "lib/dataio.h"
#include "test/test.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main(){
    srand(time(NULL));
    char* dir = "../temp";
    generate_random_data(100,10,dir);

    CellKey* cellkey = read_public_keys("../temp/keys.txt");
    print_list_keys(cellkey);
    TEST_SUMMARY();
}