#ifndef _ERROR_H
#define _ERROR_H
#include <stdio.h>

/**
 * @file error.h
 * @brief Functions to handle errors message.
 */

#define MALLOC_ERROR(msg) fprintf(stderr, "malloc error at %d in file %s in function %s\n message : %s \n", __LINE__, __FILE__, __func__, (msg));
#define FILE_ERROR(msg) fprintf(stderr, "file error at %d in file %s in function %s\n message : %s\n", __LINE__, __FILE__, __func__, (msg));
#endif