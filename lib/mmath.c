#include "mmath.h"
#include <stdlib.h>
#include <stdbool.h>



/**
 * @brief checks if p is prime.
 * complexity O(p), certainty 100%
 * 
 * @param p 
 * @return true : p is prime
 * @return false : p is not prime
 */
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

/**
 * @brief generates a random long inside [low, up]
 * 
 * @param low lower bound (included)
 * @param up upper bound (included)
 * @return long 
 */
long rand_long(long low, long up) {
    if(low > up) return low;
    return rand() % (up - low + 1) + low;
}

/**
 * @brief checks if p is prime with (1/4)^k probability of a false positive.
 * the larger the k, the slower the calculation but higher certainty.
 * complexity is O(k)
 * 
 * @param p potential prime
 * @param k 
 * @return true : p is prime with 1 - (1/4)^k certainty
 * @return false : p isn't prime with 100% certainty
 */
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
        if (witness(a, b, d, p)) 
            return false;
    }
    return true;
}

/**
 * @brief Generates a random prime number with number of bits
 * between low_size and up_size.
 * k specifies the certainty of the result being prime.
 * There is a (1/4)^k probability of a false positive.
 * The greater the value of k, the slower the calculations.
 * returns -1 if a prime wasn't found...
 * 
 * @param low_size 
 * @param up_size 
 * @param k 
 * @return long 
 */
long random_prime_number(int low_size, int up_size, int k) {
    if (low_size > up_size) return -1;
    low_size--;
    up_size--;
    // 0010000 -1 deviens 0001111
    // puis on le and avec 0010000 et cela devient donc 0011111
    // on fait expres de ne pas utiliser (1<<(up_size+1))-1 pour le 
    // cas special ou up_size = nombre de bits du entier 
    // (dans ce cas le bit up_size+1 depasserait la taille du entier)
    const long up_bound = ((1 << up_size) - 1) | (1 << up_size);
    // toutes les valeurs de 001000 a 001111
    for (long X = (1 << low_size); X < up_bound; X++) {
        if (is_prime_miller(X, k)) return X;
    }
    return -1;
}

/**
 * @brief calculates the exponent of a to the power 
 * of m modulo n in O(m^2). 
 * (a ^ m) % n = x
 * 
 * @param a 
 * @param m 
 * @param n 
 * @return int : the value of x
 */
long modpow_naive(long a, long m, long n) {
    long A = 1;

    if (m == 0) return 1 % n;

    for (int i = 0; i < m; i++) {
        A *= a;
        A %= n;
    }

    return A;
}

/**
 * @brief calculates the exponent of a to the power 
 * of m modulo n in O(m log m). this is the main implementation
 * (a ^ m) % n = x
 * 
 * @param a 
 * @param m 
 * @param n 
 * @return long : the value of x
 */
long modpow(long a, long m, long n) {
    return modpow_r(a, m, n);
    /// TODO: actually implement the iterative verstion...

    // a %= n;
    // if(m == 0) return 1;
    // if(m == 1) return a;
    // long res = 1;
    // while(m > 0){
    //     if(m & 1)
    //         res = (res * a) % n;
    //     // if(m == 1) break;
    //     printf("  : %ld %ld\n", m, res);
    //     m = m >> 1;
    //     res = (res * res) % n;
    //     printf("  %ld %ld\n", m, res);
    // }
    // printf("%ld\n", res);
    // return res;
}

/**
 * @brief calculates the exponent of a to the power 
 * of m modulo n in O(m log m). _r stands for recursive
 * (a ^ m) % n = x
 * 
 * @param a 
 * @param m 
 * @param n 
 * @return long : the value of x
 */
long modpow_r(long a, long m, long n) {
    a %= n;
    if (m == 0) return 1;
    if (m == 1) return a;
    if (m % 2 == 0) {
        long x = modpow_r(a, m / 2, n);
        return (x * x) % n;
    }
    else {
        long x = modpow_r(a, m / 2, n);
        return (((x * x) % n) * a) % n;
    }
}


/**
 * @brief calculates the pgcd and the 
 * corresponding Bezout decomposition.
 * pgcd = s*u + t*v
 * 
 * @param s pgcd(s, _)
 * @param t pgcd(_, t)
 * @param u return value
 * @param v return value
 * @return long : the value of pgcd
 */
long extended_gcd(long s, long t, long* u, long* v) {
    if (t == 0) {
        *u = 1;
        *v = 0;
        return s;
    }
    long uPrim, vPrim;
    long gcd = extended_gcd(t, s % t, &uPrim, &vPrim);
    *u = vPrim;
    *v = uPrim - (s / t) * vPrim;
    return gcd;
}