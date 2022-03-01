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

#define TEST(CALL, RES){\
    _testcase_count++; \
    _section_count++; \
    if((CALL) == (RES)){ \
        printf(_OK " test case: %3d passed successfully! \n", _testcase_count); \
    }else{ \
        fprintf(stderr, _NOK " test case: %3d failed! ["__FILE__ " | line: %d]\n", _testcase_count, __LINE__); \
        fprintf(stderr, _NLF "   inside call \"%s\"\n", #CALL); \
        fprintf(stderr, _NLF "   expected result was \"%s\"\n", #RES); \
        _testcase_failed++; \
        _section_failed++; \
    } \
}

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

#define TEST_SUMMARY(){ \
    printf("\n==============================================\n"); \
    if(_testcase_failed == 0){ \
        printf(_OK " : all "__FILE__" tests passed successfully! \n"); \
    }else{ \
        printf(_NOK " : %d out of %d tests failed! \n", _testcase_failed, _testcase_count); \
    } \
    printf("==============================================\n\n\n"); \
}

#define TEST_SECTION(NAME){ \
    printf("==== " #NAME "\n"); \
    _c_section = #NAME; \
    _section_count = 0; \
    _section_failed = 0; \
}

#define TEST_SECTION_END(){ \
    if(_section_failed == 0){ \
        printf("==== %s " _OK " : all tests passed successfully! \n", _c_section); \
    }else{ \
        printf("==== %s " _NOK " : %d out of %d tests failed! \n", _c_section, _section_failed, _section_count); \
    } \
}

#endif