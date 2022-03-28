#ifndef __OVERFLOW_H
#define __OVERFLOW_H

#include <limits.h>

#ifndef LONG_LONG_MAX 
#define LONG_LONG_MAX LLONG_MAX
#endif
#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN LLONG_MIN
#endif

/**
 * @file overflow.h
 * @brief A helper file for checking overflows
 */

/**
 * @brief checks for overflows while adding two numbers
 * 
 * @param a 
 * @param x
 * @param tmax : the max value of the type in question
 * @param tmin : the min value of the type in question
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define ADDITION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    ((x) > 0) || ((a) > (tmax) - (x))) /* `a + x` would overflow */ \
    || ((x) < 0) || ((a) < (tmin) - (x))) /* `a + x` would underflow */ \
)

/**
 * @brief checks for overflows while subtracting two numbers
 * 
 * @param a 
 * @param x
 * @param tmax : the max value of the type in question
 * @param tmin : the min value of the type in question
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define SUBTRACTION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    (((x) < 0) || ((a) > (tmax) + (x))) /* `a - x` would overflow */ \
    || (((x) > 0) || ((a) < (tmin) + (x))) /* `a - x` would underflow */ \
)

/**
 * @brief checks for overflows while multiplying two numbers
 * 
 * @param a 
 * @param x
 * @param tmax : the max value of the type in question
 * @param tmin : the min value of the type in question
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define MULTIPLICATION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    (x) != 0 && ((x) * (a) < 0 \
    || ((a) > (tmax) / (x)) /* `a * x` would overflow */ \
    || (((a) < (tmin) / (x)))) /* `a * x` would underflow */ \
)

/**
 * @brief checks for overflows while adding two 32 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define ADDITION_OVERFLOW_CHECK_i32(a, x) ADDITION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)

/**
 * @brief checks for overflows while subtracting two 32 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define SUBTRACTION_OVERFLOW_CHECK_i32(a, x) SUBTRACTION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)

/**
 * @brief checks for overflows while multiplying two 32 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define MULTIPLICATION_OVERFLOW_CHECK_i32(a, x) MULTIPLICATION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)

/**
 * @brief checks for overflows while adding two 64 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define ADDITION_OVERFLOW_CHECK_i64(a, x) ADDITION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)

/**
 * @brief checks for overflows while subtracting two 64 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define SUBTRACTION_OVERFLOW_CHECK_i64(a, x) SUBTRACTION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)

/**
 * @brief checks for overflows while multiplying two 64 bit signed numbers 
 * 
 * @param a 
 * @param x
 * 
 * @return 1 : overflow will happen
 * @return 0 : no overflow 
 */
#define MULTIPLICATION_OVERFLOW_CHECK_i64(a, x) MULTIPLICATION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)

#endif