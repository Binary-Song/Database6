#ifndef EVAL_H
#define EVAL_H
#include "basic_linked_lists.h"
#include "stdbool.h"
typedef struct Op
{
    const char *name;
    int priority;
    int unary;
    char *(*func)(const char *, const char *);
} Op;

DECLARE_LIST(Op)

typedef struct Func
{
    const char *name;
    char *(*func)(const char *);
} Func;

DECLARE_LIST(Func)

typedef struct Var
{
    const char *name;
    const char *value;
} Var;

DECLARE_LIST(Var)

typedef struct Element
{
    int type;
    char *literal;
    int operator_kind;
} Element;

DECLARE_LIST(Element)

typedef struct Calc
{
    List(Op) * operators;
    List(Var) * vars;
    List(Func) * functions;
} Calc;

char *eval_e(Calc calc, List(Element) * _elems, bool *error);

char *eval(Calc calc, const char *expr);

Op initOp(const char *name, char *(*func)(const char *, const char *), int p, bool unary);

Func initFunc(const char *name, char *(*func)(const char *));

Var initVar(const char *name, const char *value);
#endif