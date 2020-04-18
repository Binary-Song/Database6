#ifndef MEMCHECK_H
#define MEMCHECK_H
#include "stdlib.h"

#define new(a) mnew(a, __FILE__, __LINE__)
#define newmem(a, b) mnew((a) * (b), __FILE__, __LINE__)
#define delete mdelete

extern int next_memid;

void memcheck_begin();

void memcheck_end();

void *mnew(size_t size, char *file, int line);

void mdelete(void *);

#endif