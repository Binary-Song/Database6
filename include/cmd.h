#ifndef CMD_H
#define CMD_H
#include "memcheck.h"
#include "basic_linked_lists.h"
typedef struct _Pair
{
    string key;
    string value;
} Pair;

typedef struct _Tag
{
    string value;
} Tag;

DECLARE_LIST(Pair)
DECLARE_LIST(Tag)

void cmd_add_record(List(string) *);
void cmd_add_field(List(Pair) *, List(Tag) *);
void cmd_list_fields(List(Pair) * pairs, List(Tag) * tags);
void cmd_list_records(List(Pair) * pairs, List(Tag) * tags);
void cmd_remove_record(List(Pair) * pairs, List(Tag) * tags);
void cmd_configure_field(List(Pair) * pairs, List(Tag) * tags); 
#endif