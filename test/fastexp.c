#include "test.h"

int main(){
    TEST(modpow_naive(3, 3, 11), 5);
    TEST(modpow_naive(2, 10, 69), 58);

    TEST(modpow(3, 3, 11), 5);
    TEST(modpow(2, 10, 69), 58);

    TEST_SUMMARY(); 
}