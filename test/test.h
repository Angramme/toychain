#ifndef _TEST_H
#define _TEST_H

#include <stdio.h>

int _testcase_count = 0;
int _testcase_failed = 0;
int _section_failed = 0;
int _section_count = 0;
char* _c_section = "error";

#define _OK " OK "
#define _NOK "ERR!"
#define _NLF " :: "

/**
 * @file test.h
 * @brief A collection of defines for simplified testing
 * 
 */

/**
 * @brief tests if CALL is equal to RES and makes according output
 * 
 * @param CALL (any) : the call to test
 * @param RES (any) : the output to compare
 */
#define TEST(CALL, RES){\
    _testcase_count++; \
    _section_count++; \
    if((CALL) == (RES)){ \
        printf(_OK " test case: %3d passed successfully! \n", _testcase_count); \
    }else{ \
        printf(_NOK " test case: %3d failed! \n", _testcase_count); \
        fprintf(stderr, _NOK " test case: %3d failed! ["__FILE__ " | line: %d]\n", _testcase_count, __LINE__); \
        fprintf(stderr, _NLF "   inside call \"%s\"\n", #CALL); \
        fprintf(stderr, _NLF "   expected result was \"%s\"\n", #RES); \
        _testcase_failed++; \
        _section_failed++; \
    } \
}

/**
 * @brief tests if CALL is equal to RES and makes according output
 * in the case of an error, outputs the error message
 * 
 * @param CALL (any) : the call to test
 * @param RES (any) : the output to compare
 * @param ERRMSG string : the error message to output
 */
#define TEST_MSG(CALL, RES, ERRMSG){\
    _testcase_count++; \
    _section_count++; \
    if((CALL) == (RES)){ \
        printf(_OK " test case: %3d passed successfully! \n", _testcase_count); \
    }else{ \
        fprintf(stderr, _NOK " test case: %3d failed! ["__FILE__ " | line: %d]\n", _testcase_count, __LINE__); \
        fprintf(stderr, _NLF "   inside call \"%s\"\n", #CALL); \
        fprintf(stderr, _NLF "   expected result was \"%s\"\n", #RES); \
        fprintf(stderr, _NLF "   error message: %s \n", ERRMSG); \
        _testcase_failed++; \
        _section_failed++; \
    } \
}

/**
 * @brief outputs the test summary for the file
 */
#define TEST_SUMMARY(){ \
    printf("\n==============================================\n"); \
    if(_testcase_failed == 0){ \
        printf(_OK " : all "__FILE__" tests passed successfully! \n"); \
    }else{ \
        printf(_NOK " : %d out of %d " __FILE__ " tests failed! \n", _testcase_failed, _testcase_count); \
    } \
    printf("==============================================\n\n\n"); \
}

/**
 * @brief beggining of a section of tests
 * 
 * @param NAME string : the name of the section
 */
#define TEST_SECTION(NAME){ \
    printf("\n==== starting " #NAME " tests\n"); \
    _c_section = #NAME; \
    _section_count = 0; \
    _section_failed = 0; \
}

/**
 * @brief end of a section of tests
 */
#define TEST_SECTION_END(){ \
    if(_section_failed == 0){ \
        printf("---- %s " _OK " : all tests passed successfully! \n", _c_section); \
    }else{ \
        printf("---- %s " _NOK " : %d out of %d tests failed! \n", _c_section, _section_failed, _section_count); \
    } \
}

#endif