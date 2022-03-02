#ifndef __CLOCK_TIMEHH
#define __CLOCK_TIMEHH

#include <time.h>

/**
 * @file clocktime.h
 * @brief A simple define for timing the performance
 * 
 */

/**
 * @brief Simple timing function
 * 
 * @param OUT double : the output variable
 * @param FNC_CALL (any) : the function call to time
 */
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