#ifndef MYMALLOC_H /* Include guard */
#define MYMALLOC_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.c"

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void mycleanup();

#endif // MYMALLOC_H