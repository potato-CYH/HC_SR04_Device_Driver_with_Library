#ifndef _COMMON_
#define _COMMON_

#include "hcsr04.c"

int init();
float measure(int fd);
void finalization(int fd);

#endif

