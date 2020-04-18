#include "eval_lib.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#undef log
#include "math.h"
#define ERROR 0.00001

double to_double(const char *str, bool *success)
{
    if (!str)
    {
        return 0;
    }
    char *endp;
    double result = strtod(str, &endp);
    if (!(*endp)) //转到底，正确
    {
        return result;
    }
    *success = false;
    return 0;
}

#pragma region ARITHMETICS 四则运算

char *OP_plus(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        char *res = calloc(1, strlen(a) + strlen(b) + 1);
        strcat(res, a);
        strcat(res, b);
        return res;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.3lf", na + nb);
    return res;
}
char *OP_minus(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.3lf", na - nb);
    return res;
}
char *OP_mult(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.3lf", na * nb);
    return res;
}
char *OP_div(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.3lf", na / nb);
    return res;
}

#pragma endregion

#pragma region COMPARISON 比较
#define EQ(na, nb) (nb - na <= ERROR && na - nb <= ERROR)
char *OP_greater(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (na - nb > ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");

    return res;
}
char *OP_less(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (nb - na > ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");
    return res;
}
char *OP_equal(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    char *res = calloc(1, 20);
    if (!success)
    {
        if (!strcmp(a, b))
        {
            snprintf(res, 19, "1");
        }
        else
        {
            snprintf(res, 19, "0");
        }
        return res;
    }
    if (nb - na <= ERROR && na - nb <= ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");
    return res;
}
char *OP_greater_or_eq(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }

    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (na - nb > -ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");

    return res;
}
char *OP_less_or_eq(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (nb - na > -ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");
    return res;
}
char *OP_not_equal(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    char *res = calloc(1, 20);
    if (!success)
    {
        if (strcmp(a, b))
        {
            snprintf(res, 19, "1");
        }
        else
        {
            snprintf(res, 19, "0");
        }
        return res;
    }
    if (nb - na > ERROR || na - nb > ERROR)
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");
    return res;
}
#pragma endregion

#pragma region STATISTICS 统计
char *FX_floor(const char *a)
{
    if (!a)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.0lf", floor(na));
    return res;
}

char *FX_round(const char *a)
{
    if (!a)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    snprintf(res, 19, "%.0lf", round(na));
    return res;
}

#pragma endregion

#pragma region LOGICS 逻辑
char *OP_and(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (!EQ(na, 0) && !EQ(nb, 0))
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");

    return res;
}
char *OP_or(const char *a, const char *b)
{
    if (!a || !b)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值
    double nb = to_double(b, &success);
    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (!EQ(na, 0) || !EQ(nb, 0))
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");

    return res;
}
char *OP_not(const char *a, const char *b)
{
    if (!a)
    {
        return NULL;
    }
    bool success = true;
    double na = to_double(a, &success); //只有失败时才会改success的值

    if (!success)
    {
        return NULL;
    }
    char *res = calloc(1, 20);
    if (EQ(na, 0))
        snprintf(res, 19, "1");
    else
        snprintf(res, 19, "0");

    return res;
}
#pragma endregion

List(Op) * GetLibOps()
{
    List(Op) *ops = list_create(Op)(NULL);
    list_append(Op)(ops, initOp("+", OP_plus, 4, false));
    list_append(Op)(ops, initOp("-", OP_minus, 4, true));
    list_append(Op)(ops, initOp("*", OP_mult, 3, false));
    list_append(Op)(ops, initOp("/", OP_div, 3, false));
    list_append(Op)(ops, initOp(">", OP_greater, 6, false));
    list_append(Op)(ops, initOp("<", OP_less, 6, false));
    list_append(Op)(ops, initOp("==", OP_equal, 7, false));
    list_append(Op)(ops, initOp("=", OP_equal, 7, false));
    list_append(Op)(ops, initOp("!=", OP_not_equal, 7, false));
    list_append(Op)(ops, initOp(">=", OP_greater_or_eq, 6, false));
    list_append(Op)(ops, initOp("<=", OP_less_or_eq, 6, false));
    list_append(Op)(ops, initOp("&&", OP_and, 11, false));
    list_append(Op)(ops, initOp("||", OP_or, 12, false));
    list_append(Op)(ops, initOp("!", OP_not, 2, true));
    return ops;
}

List(Func) * GetLibFuncs()
{
    List(Func) *funcs = list_create(Func)(NULL);
    list_append(Func)(funcs, initFunc("floor", FX_floor));
    list_append(Func)(funcs, initFunc("round", FX_round));
    return funcs;
}

List(Var) * GetLibVars()
{
    List(Var) *vars = list_create(Var)(NULL);
    list_append(Var)(vars, initVar("PI", "3.1415"));
    return vars;
}

Calc init_std_calc()
{
    Calc c;
    c.functions = GetLibFuncs();
    c.operators = GetLibOps();
    c.vars = GetLibVars();
    return c;
}

void deinit_calc(Calc calc)
{
    list_delete(Func)(calc.functions);
    list_delete(Var)(calc.vars);
    list_delete(Op)(calc.operators);
}
