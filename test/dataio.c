#include "lib/dataio.h"
#include "test/test.h"
#include "lib/error.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

void test_read_write_protected(){
    srand(time(NULL));
    char* dir = "../temp/";
    char path[500];
    generate_random_data(30,10,dir);

    strcpy(path, dir);
    strcat(path, "declarations.txt");
    CellProtected* plist = read_protected(path);
    print_protected_list(plist);
    free_list_protected(plist);

    strcpy(path, dir);
    strcat(path, "keys.txt");
    CellKey* cellkey = read_public_keys(path);
    print_list_keys(cellkey);
    free_list_keys(cellkey);
}

int main(){

    TEST_SECTION(list_protected_to_str and str_to_list_protected);
    CellProtected* randlist = rand_list_protected(5);
    if(!randlist){
        fprintf(stderr, "randlist is NULL\n");
        return -1;
    }
    char* str = list_protected_to_str(randlist);
    if(!str){
        fprintf(stderr, "str is NULL\n");
        free_list_protected(randlist);
        return -1;
    }
    CellProtected* list2 = str_to_list_protected(str);
    if(!list2){
        fprintf(stderr, "lsit2 is NULL\n");
        free(str);
        free_list_protected(randlist);
        return -1;
    }
    bool good = true;
    CellProtected* it1 = randlist;
    CellProtected* it2 = list2;
    while (it1 && it2){
        if(!it1){
            good = !it2;
            break;
        }
        good = good && (strcmp(it1->data->msg, it2->data->msg) == 0);
        good = good && (it1->data->pKey->n == it2->data->pKey->n);
        good = good && (it1->data->pKey->v == it2->data->pKey->v);
        good = good && (it1->data->sig->len == it2->data->sig->len);
        if(!good) break;
        size_t i;
        for(i=0; i<it1->data->sig->len; i++)
            good = good && (it1->data->sig->xs[i] == it2->data->sig->xs[i]);
        it1 = it1->next;
        it2 = it2->next;
    }
    free_list_protected(randlist);
    free_list_protected(list2);
    free(str);
    TEST(good, true);
    TEST_SECTION_END();

    // test_read_write_protected();
    
    TEST_SUMMARY();
}