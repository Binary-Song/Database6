#ifndef UTF8_H
#define UTF8_H
#include "memcheck.h"
#include "basic_linked_lists.h"
#include "stdbool.h"
int byte_count(const char * );

List(string)* split(const char *m);

void fixed_print(const char *m, int fixed_length, bool right);

void np(const char* a);
#endif