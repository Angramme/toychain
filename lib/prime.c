#include "prime.h"
#include "fastexp.h"
#include <stdlib.h>

bool is_prime_naive(long p) {
    for (long i = 3; i < p; i++) {
        if (p % i == 0) return false;
    }
    return true;
}



bool witness(long a, long b, long d, long p) {
    long x = modpow(a, d, p);
    if (x == 1) {
        return false;
    }
    for (long i = 0; i < b; i++) {
        if (x == p - 1) {
            return false;
        }
        x = modpow(x, 2, p);
    }
    return true;
}

long rand_long(long low, long up) {
    return rand() % (up - low + 1) + low;
}

bool is_prime_miller(long p, int k) {
    if (p == 2) {
        return true;
    }
    if (!(p & 1) || p <= 1) { //on verifie que p est impair et different de 1
        return false;
    }
    //on determine b et d :
    long b = 0;
    long d = p - 1;
    while (!(d & 1)) { //tant que d n’est pas impair
        d = d / 2;
        b = b + 1;
    }
    // On genere k valeurs pour a, et on teste si c’est un temoin :
    long a;
    int i;
    for (i = 0; i < k; i++) {
        a = rand_long(2, p - 1);
        if (witness(a, b, d, p)) {
            return false;
        }
    }
    return true;
}


/**
 * returns -1 if a prime wasn't found...
*/
long random_prime_number(int low_size, int up_size, int k){
    if(low_size > up_size) return -1;
    low_size--;
    up_size--;
    // 0010000 -1 deviens 0001111
    // puis on le and avec 0010000 et cela devient donc 0011111
    // on fait expres de ne pas utiliser (1<<(up_size+1))-1 pour le 
    // cas special ou up_size = nombre de bits du entier 
    // (dans ce cas le bit up_size+1 depasserait la taille du entier)
    const long up_bound = ((1<<up_size)-1) | (1<<up_size); 
    // toutes les valeurs de 001000 a 001111
    for(long X = (1<<low_size); X < up_bound; X++){
        if(is_prime_miller(X, k)) return X;
    }
    return -1;
}
