#include "fastexp.h"


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
    long A = a;
    if(m == 0) return 1%n;
    while(m > 1){
        if(m % 2 == 0){
            m /= 2;
            A = (A*A)%n;
        }else{
            A *= a;
            A %= n;
            m -= 1;
        }
    }
}