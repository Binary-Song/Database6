#include "memcheck.h"
#define DECALRE_STACK(T)                                         \
    typedef struct StackNode##T                                  \
    {                                                            \
        struct StackNode##T *inner;                              \
        T data;                                                  \
    } StackNode##T;                                              \
    typedef struct Stack##T                                      \
    {                                                            \
        StackNode##T *top;                                       \
        int count;                                               \
        void (*dealloc)(T);                                      \
        T(*copy)                                                 \
        (T);                                                     \
    } Stack##T;                                                  \
    Stack##T *stack_create##T(void (*dealloc)(T), T (*copy)(T)); \
    StackNode##T *stack_push##T(Stack##T *stack, T data);        \
    void stack_delete##T(Stack##T *stack);                       \
    int stack_count##T(Stack##T *stack);                         \
    T stack_peek##T(Stack##T *stack, int layer);                    \
    T stack_pop##T(Stack##T *stack);

#define DEFINE_STACK(T)                                                    \
    Stack##T *stack_create##T(void (*dealloc)(T), T (*copy)(T))            \
    {                                                                      \
        Stack##T *stack = newmem(sizeof(Stack##T), 1);                     \
        stack->count = 0;                                                  \
        stack->dealloc = dealloc;                                          \
        stack->copy = copy;                                                \
        stack->top = NULL;                                                 \
        return stack;                                                      \
    }                                                                      \
    StackNode##T *stack_push##T(Stack##T *stack, T data)                   \
    {                                                                      \
        if (!stack)                                                        \
        {                                                                  \
            fprintf(stderr, "[ERROR](STACK): Unexpected null pointer.\n"); \
            exit(1);                                                       \
        }                                                                  \
        StackNode##T *node = newmem(sizeof(StackNode##T), 1);              \
        node->data = data;                                                 \
        node->inner = stack->top;                                          \
        stack->count++;                                                    \
        stack->top = node;                                                 \
        return node;                                                       \
    }                                                                      \
    void stack_delete##T(Stack##T *stack)                                  \
    {                                                                      \
        if (!stack)                                                        \
        {                                                                  \
            fprintf(stderr, "[ERROR](STACK): Unexpected null pointer.\n"); \
            exit(1);                                                       \
        }                                                                  \
        StackNode##T *curr = stack->top;                                   \
        StackNode##T *inner;                                               \
        while (curr)                                                       \
        {                                                                  \
            inner = curr->inner;                                           \
            if (stack->dealloc)                                            \
            {                                                              \
                stack->dealloc(curr->data);                                \
            }                                                              \
            delete(curr);                                                    \
            curr = inner;                                                  \
        }                                                                  \
        delete(stack);                                                       \
    }                                                                      \
    T stack_pop##T(Stack##T *stack)                                        \
    {                                                                      \
        if (!stack || !stack->top)                                         \
        {                                                                  \
            fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");  \
            exit(1);                                                       \
        }                                                                  \
        T t = stack->copy(stack->top->data);                               \
        stack->dealloc(stack->top->data);                                  \
        StackNode##T *newtop = stack->top->inner;                          \
        delete(stack->top);                                                  \
        stack->top = newtop;                                               \
        stack->count--;                                                    \
        return t;                                                          \
    }                                                                      \
    T stack_peek##T(Stack##T *stack, int layer)                            \
    {                                                                      \
        StackNode##T *node = stack->top;                                   \
        while (layer)                                                      \
        {                                                                  \
            layer--;                                                       \
            node = node->inner;                                            \
        }                                                                  \
        return node->data;                                                 \
    }                                                                      \
    int stack_count##T(Stack##T *stack) { return stack->count; }

#define Stack(T) Stack##T
#define StackNode(T) StackNode##T
#define stack_create(T) stack_create##T
#define stack_push(T) stack_push##T
#define stack_delete(T) stack_delete##T
#define stack_count(T) stack_count##T
#define stack_peek(T) stack_peek##T
#define stack_pop(T) stack_pop##T