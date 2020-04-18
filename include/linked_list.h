/*---------------- BEGIN OF GENERATED CODES ----------------*/



/*---------------- DECLARATION OF STRUCTS AND FUNCTIONS, PUT THEM IN HEADERS ----------------*/



#define DECLARE_LIST(T)\
\
typedef struct ListNode##T\
{\
    struct ListNode##T *prev;\
    struct ListNode##T *next;\
    T data;\
} ListNode##T;\
\
typedef struct List##T\
{\
    ListNode##T *head;\
    ListNode##T *iter;\
    void (*dealloc)(T);\
    int count;\
} List##T;\
\
List##T *list_create##T(void (*dealloc)(T));\
\
ListNode##T *list_append##T(List##T *list, T data);\
\
void list_foreach##T(List##T *list, void (*func)(T));\
\
void list_delete##T(List##T *list);\
\
void list_remove##T(List##T *list, int index, int count);\
\
ListNode##T *list_insert##T(List##T *list, int index, T data);\
\
int list_count##T(List##T *list);\
\
T list_get_from_node_pointer##T(ListNode##T *pointer);\
\
T list_get##T(List##T *list, int index);\
\
ListNode##T *list_get_node_pointer##T(List##T *list, int index);\
\
void list_set##T(List##T *list, int index, T newvalue);\




/*---------------- IMPLEMENTATION OF STRUCTS AND FUNCTIONS, PUT THEM IN SOURCES ----------------*/



#define DEFINE_LIST(T)\
\
\
List##T *list_create##T(void (*dealloc)(T))\
{\
    List##T *list = calloc(sizeof(List##T), 1);\
    list->head = calloc(sizeof(ListNode##T), 1);\
    list->dealloc = dealloc;\
    list->count = 0;\
    return list;\
}\
\
ListNode##T *list_append##T(List##T *list, T data)\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    ListNode##T *node = list->head;\
    while (node->next)\
    {\
        node = node->next;\
    }\
    node->next = calloc(sizeof(ListNode##T), 1);\
    node->next->prev = node;\
    node->next->data = data;\
    list->count++;\
    return node->next;\
}\
\
void list_foreach##T(List##T *list, void (*func)(T))\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    ListNode##T *node = list->head;\
    while (node->next)\
    {\
        node = node->next;\
        func(node->data);\
    }\
}\
\
void list_delete##T(List##T *list)\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    ListNode##T *curr = list->head->next;\
    ListNode##T *next;\
    while (curr)\
    {\
        next = curr->next;\
        if (list->dealloc)\
        {\
            list->dealloc(curr->data);\
        }\
        free(curr);\
        curr = next;\
    }\
    free(list);\
}\
\
void list_remove##T(List##T *list, int index, int count)\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    if (index < 0)\
    {\
        fprintf(stderr, "[ERROR](LIST): Invalid index.\n");\
        exit(EXIT_FAILURE);\
    }\
    if (count < 0)\
    {\
        fprintf(stderr, "[ERROR](LIST): Invalid count.\n");\
        exit(EXIT_FAILURE);\
    }\
\
    ListNode##T *node_before_range = NULL;\
    ListNode##T *node = list->head;\
    int i = -1;\
    while (node->next)\
    {\
        node = node->next;\
        ++i;\
\
        if (index == i)\
        {\
            node_before_range = node->prev;\
            while (count && node)\
            {\
                ListNode##T *next = node->next;\
                if (list->dealloc)\
                {\
                    list->dealloc(node->data);\
                }\
                free(node);\
                node = next;\
                --count;\
                list->count--;\
            }\
            if (count && !node)\
            {\
                fprintf(stderr, "[ERROR](LIST): Out of range.\n");\
                exit(EXIT_FAILURE);\
            }\
            node_before_range->next = node;\
            if (node)\
            {\
                node->prev = node_before_range;\
            }\
            return;\
        }\
    }\
}\
\
ListNode##T *list_insert##T(List##T *list, int index, T data)\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    if (index < 0)\
    {\
        fprintf(stderr, "[ERROR](LIST): Invalid index.\n");\
        exit(EXIT_FAILURE);\
    }\
\
    int i = -1;\
    ListNode##T *node = list->head;\
    while (node->next)\
    {\
        node = node->next;\
        ++i;\
\
        if (i == index)\
        {\
            ListNode##T *new_node = malloc(sizeof(ListNode##T));\
            new_node->data = data;\
\
            ListNode##T *prev = node->prev;\
            ListNode##T *mid = new_node;\
            ListNode##T *next = node;\
\
            mid->prev = prev;\
            mid->next = next;\
\
            prev->next = mid;\
            next->prev = mid;\
\
            list->count++;\
            return new_node;\
        }\
    }\
    if (i == index - 1)\
    {\
        ListNode##T *new_node = malloc(sizeof(ListNode##T));\
        new_node->data = data;\
\
        new_node->next = NULL;\
        new_node->prev = node;\
        node->next = new_node;\
\
        list->count++;\
        return new_node;\
    }\
    fprintf(stderr, "[ERROR](LIST): Out of range.\n");\
    exit(EXIT_FAILURE);\
}\
\
int list_count##T(List##T *list)\
{\
    return list->count;\
}\
\
T list_get_from_node_pointer##T(ListNode##T *pointer)\
{\
    return pointer->data;\
}\
\
T list_get##T(List##T *list, int index)\
{\
    if (index < 0)\
    {\
        fprintf(stderr, "[ERROR](LIST): Out of range.\n");\
        exit(EXIT_FAILURE);\
    }\
    ListNode##T *node = list->head;\
    int i = -1;\
    while (node->next)\
    {\
        node = node->next;\
        i++;\
        if (i == index)\
        {\
            return node->data;\
        }\
    }\
    fprintf(stderr, "[ERROR](LIST): Out of range.\n");\
    exit(EXIT_FAILURE);\
}\
\
ListNode##T *list_get_node_pointer##T(List##T *list, int index)\
{\
    if (!list || !list->head)\
    {\
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");\
        exit(EXIT_FAILURE);\
    }\
    if (index < 0)\
    {\
        fprintf(stderr, "[ERROR](LIST): Invalid index.\n");\
        exit(EXIT_FAILURE);\
    }\
    ListNode##T *node = list->head;\
    int i = -1;\
    while (node->next)\
    {\
        node = node->next;\
        i++;\
        if (i == index)\
        {\
            return node;\
        }\
    }\
    fprintf(stderr, "[ERROR](LIST): Out of range.\n");\
    exit(EXIT_FAILURE);\
}\
\
void list_set##T(List##T *list, int index, T newvalue)\
{\
    ListNode##T *node = list->head;\
    int i = -1;\
    while (node->next)\
    {\
        node = node->next;\
        i++;\
        if (list->dealloc&& i == index)\
        {\
            list->dealloc(node->data);\
            node->data = newvalue;\
            return;\
        }\
    }\
} 



/*---------------- END OF GENERATED CODES ----------------*/



#define List(T) List##T
#define ListNode(T) ListNode##T
#define list_append(T) list_append##T
#define list_create(T) list_create##T
#define list_foreach(T) list_foreach##T
#define list_delete(T) list_delete##T
#define list_remove(T) list_remove##T
#define list_insert(T) list_insert##T
#define list_count(T) list_count##T
#define list_get_from_node_pointer(T) list_get_from_node_pointer##T
#define list_get(T) list_get##T
#define list_get_node_pointer(T) list_get_node_pointer##T
#define list_set(T) list_set##T
#define Foreach(T, v, l) for (l->iter = l->head; l->iter->next && ((v = l->iter->next->data), 1); l->iter = l->iter->next)
#include <stdio.h>
#include <stdlib.h>