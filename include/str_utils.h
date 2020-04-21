#ifndef STRING_UTILS
#define STRING_UTILS
#include "memcheck.h"
#include <stdbool.h>
typedef char *string;
typedef const char *const_string;
string string_duplicate(const_string str);
void string_dealloc(string str);
bool string_to_bool(string str, bool *success);
string path_prefix(string str);
#include "messenger.h"
#define NS(s) (s ? s : C_NULL "[null]" C_RESET)
#define NS_LOG(s) (s ? s : C_LOG_NULL "[null]" C_LOG_RESET)
#endif