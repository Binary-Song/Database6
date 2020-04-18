#ifndef EVAL_LIB_H
#define EVAL_LIB_H
#include "eval.h"
#include "stdbool.h"

Calc init_std_calc();

void deinit_calc(Calc calc);

List(Op) * GetLibOps();

List(Func) * GetLibFuncs();

List(Var) * GetLibVars();

#endif