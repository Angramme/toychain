#ifndef _ERROR_H
#define _ERROR_H

#include <stdio.h>

#define MALLOC_ERROR(msg) fprintf(stderr, "malloc error at %d in file %s \n  message: %s \n", __LINE__, __FILE__, (msg));

#endif