#ifndef __OVERFLOW_H
#define __OVERFLOW_H

#include <limits.h>

#ifndef LONG_LONG_MAX 
#define LONG_LONG_MAX LLONG_MAX
#endif
#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN LLONG_MIN
#endif

#define ADDITION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    ((x) > 0) || ((a) > (tmax) - (x))) /* `a + x` would overflow */ \
    || ((x) < 0) || ((a) < (tmin) - (x))) /* `a + x` would underflow */ \
)

#define SUBTRACTION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    (((x) < 0) || ((a) > (tmax) + (x))) /* `a - x` would overflow */ \
    || (((x) > 0) || ((a) < (tmin) + (x))) /* `a - x` would underflow */ \
)

#define MULTIPLICATION_OVERFLOW_CHECK(a, x, tmax, tmin) ( \
    (x) * (a) < 0 \
    || ((a) > (tmax) / (x)) /* `a * x` would overflow */ \
    || (((a) < (tmin) / (x))) /* `a * x` would underflow */ \
)

#define ADDITION_OVERFLOW_CHECK_i32(a, x) ADDITION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)
#define SUBTRACTION_OVERFLOW_CHECK_i32(a, x) SUBTRACTION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)
#define MULTIPLICATION_OVERFLOW_CHECK_i32(a, x) MULTIPLICATION_OVERFLOW_CHECK(a, x, INT_MAX, INT_MIN)

#define ADDITION_OVERFLOW_CHECK_i64(a, x) ADDITION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)
#define SUBTRACTION_OVERFLOW_CHECK_i64(a, x) SUBTRACTION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)
#define MULTIPLICATION_OVERFLOW_CHECK_i64(a, x) MULTIPLICATION_OVERFLOW_CHECK(a, x, LONG_LONG_MAX, LONG_LONG_MIN)

#endif