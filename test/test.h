#ifndef _TEST_H
#define _TEST_H

#include <stdio.h>

int _testcase_count = 0;
int _testcase_failed = 0;

#define _OK " OK "
#define _NOK "ERR!"

#define TEST(CALL, RES){\
    _testcase_count++; \
    if((CALL) == (RES)){ \
        printf(_OK " test case: %d passed successfully!\n", _testcase_count); \
    }else{ \
        fprintf(stderr, _NOK " test case: %d failed! ["__FILE__ " | line: %d]\n", _testcase_count, __LINE__); \
        fprintf(stderr, _NOK " | inside call \"" #CALL "\"\n"); \
        _testcase_failed++; \
    } \
}

#define TEST_MSG(CALL, RES, ERRMSG){\
    _testcase_count++; \
    if((CALL) == (RES)){ \
        printf(_OK " test case: %d passed successfully!\n", _testcase_count); \
    }else{ \
        fprintf(stderr, _NOK " test case: %d failed! ["__FILE__ " | line: %d]\n", _testcase_count, __LINE__); \
        fprintf(stderr, _NOK " | inside call \"" #CALL "\"\n"); \
        fprintf(stderr, _NOK " | error message: %s \n", ERRMSG); \
        _testcase_failed++; \
    } \
}

#define TEST_SUMMARY(){ \
    printf("\n==============================================\n"); \
    if(_testcase_failed == 0){ \
        printf(_OK " : all "__FILE__" tests passed successfully! \n"); \
    }else{ \
        printf(_NOK " : %d out of %d tests failed! \n", _testcase_failed, _testcase_count); \
    } \
    printf("==============================================\n\n\n"); \
}

#endif