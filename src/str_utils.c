#include "str_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
string string_duplicate(const_string str)
{
    if (!str)
        return NULL;
    char *result = newmem(sizeof(char), strlen(str) + 1);
    strcpy(result, str);
    return result;
}
void string_dealloc(string str)
{
    delete(str);
}
bool string_to_bool(string str, bool *success)
{
    if (!str)
    {
        *success = false;
        return false;
    }

    if (!strcmp(str, "true") || !strcmp(str, "TRUE") || !strcmp(str, "1"))
    {
        return true;
    }
    else if (!strcmp(str, "false") || !strcmp(str, "FALSE") || !strcmp(str, "0"))
    {
        return false;
    }
    else
    {
        *success = false;
        return false;
    }
}
