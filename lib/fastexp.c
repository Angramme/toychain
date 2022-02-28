#include "fastexp.h"
#include <stdio.h>


long modpow_naive(long a, long m, long n){
    long A = 1;

    if(m == 0) return 1%n;

    for(int i=0; i<m; i++){
        A *= a;
        A %= n;
    }
    
    return A;
}

int modpow(long a, long m, long n){
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

int modpow_r(long a, long m, long n){
    a %= n;
    if(m == 0) return 1;
    if(m == 1) return a;
    if(m % 2 == 0){
        long x = modpow_r(a, m/2, n);
        return (x*x) % n;
    }else{
        long x = modpow_r(a, m/2, n);
        return (((x*x) % n) * a) %n;
    }
}