#ifndef __CLOCK_TIMEHH
#define __CLOCK_TIMEHH

#include <time.h>

#define CLOCK_TIME(OUT, FNC_CALL) \
{ \
    clock_t temps_initial; \
    clock_t temps_final; \
    temps_initial = clock();  \
    { FNC_CALL; } \
    temps_final = clock(); \
    (OUT) = ((double) (temps_final - temps_initial)) / CLOCKS_PER_SEC; \
}

#endif