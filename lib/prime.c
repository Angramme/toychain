#include "prime.h"

bool is_prime_naive(long p){
    for(long i = 3; i<p; i++){
        if(p % i == 0) return false;
    }
    return true;
}

