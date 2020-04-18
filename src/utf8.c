#define DOMAIN ENCODING
#include "utf8.h"
#include "messenger.h"
#include "stdio.h"
#include "basic_linked_lists.h"
#include "stdlib.h"
#include "string.h"
void log_string_array(List(string) * arr);
inline void log_string_array(List(string) * arr)
{
#ifdef DEBUG
    string str;
    Foreach(string, str, arr)
    {
        log("%s\n",  NS_LOG(str));
    }
#endif
}

inline int byte_count(const char *m)
{
    char c = *m;
    if ((c & 0x80) == 0)
    {
        return 1;
    }
    if ((c & 0xE0) == 0xC0)
    {
        return 2;
    }
    if ((c & 0xF0) == 0xE0)
    {
        return 3;
    }
    if ((c & 0xF8) == 0xF0)
    {
        return 4;
    }
    return 0;
}

List(string) * split(const char *m)
{
    List(string) *ls = list_create(string)(string_dealloc);
    while (*m)
    {
        int charsize = byte_count(m);
        char *str = calloc(charsize + 1, 1);
        strncpy(str, m, charsize);
        list_append(string)(ls, str);
        m += charsize;
    }
    log_string_array(ls);
    return ls;
}

int get_disp_length(const char *m)
{
    int r = 0;
    while (*m)
    {
        int charsize = byte_count(m);
        if (charsize == 1)
            r += 1;
        else
            r += 2;
        m += charsize;
    }
    return r;
}


void fixed_print(const char *m, int fixed_length, bool right)
{
    if (m == NULL)
    {
        if (right)
        {
            printf(""C_NULL"%*.*s"C_RESET"", fixed_length, fixed_length, "null");
        }
        else
        {
            printf(""C_NULL"%-*.*s"C_RESET"", fixed_length, fixed_length, "null");
        }
        return;
    } 

    int printed_length = 0;
    int current_length = 0;

    int diff = fixed_length - get_disp_length(m);

    if (right)
    {
        if (diff > 0)
        {
            for (size_t i = 0; i < diff; i++)
            {
                putchar(' ');
                printed_length++;
            }
        }
    }

    if (diff < 0)
    {
        fixed_length--; //最大长度减一，来放省略号
    }

    while (*m)
    {
        int charsize = byte_count(m);
        if (charsize == 1)
            current_length = 1;
        else
            current_length = 2;

        if (current_length + printed_length <= fixed_length)
        {
            printf("%*.*s", charsize, charsize, m);
            printed_length += current_length;
        }
        else
        {
            break;
        }
        m += charsize;
    }

    if (diff < 0)
    {
        printf("…"); //省略号
    }

    while (printed_length < fixed_length)
    {
        putchar(' ');
        printed_length++;
    }
}