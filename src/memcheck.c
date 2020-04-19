#include "memcheck.h"
#include "stdio.h"
#include "string.h"
typedef struct node
{
    struct node *next;
    void *mem;
    char file[100];
    int line;
    int id;
} node;

node *head;
node *tail;
int next_memid;

void memcheck_begin()
{
    next_memid = 0;
    head = calloc(1, sizeof(node));
    head->mem = NULL;
    head->next = NULL;
    tail = head;
}

void memcheck_end()
{
    node *t = head->next;
    node *n;

    while (t)
    {
        printf("id: %d  file: %s:%d\n", t->id, t->file, t->line);
        t = t->next;
    }

    t = head;

    while (t->next)
    {
        n = t->next;
        free(t);
        t = n;
    }
    free(t);
}

void *mnew(size_t size, char *file, int line)
{
    char *p = calloc(size, 1);

    tail->next = calloc(sizeof(node), 1);
    tail = tail->next;

    strncpy(tail->file, file, 39);
    tail->line = line;
    tail->id = next_memid;
    next_memid++;
    tail->mem = p;

    // node *n = head->next;
    // int i = 0;
    // while (n)
    // {
    //     i++;
    //     n = n->next;
    // }
    // printf("NewMemAlloc,MemCount=%d\n", i);

    return p;
}

void mdelete(void *p)
{
    if (!p)
        return;

    node *curr = head->next;
    node *prev = head;

    while (curr)
    {
        node *next = curr->next;
        if (curr->mem == p)
        {
            free(p);

            if (!curr->next)//如果是尾部，就要重设尾部
            {
                tail = prev;
            } 
            free(curr);
            prev->next = next;

            // node *n = head->next;
            // int i = 0;
            // while (n)
            // {
            //     i++;
            //      n = n->next;
            // }
            // printf("Memfree,MemCount=%d\n", i);

            return;
        }
        prev = curr;
        curr = next;
    }
    printf("That would have been a segfault. Double free or free something you didn't malloc.\n");
    abort();
}
