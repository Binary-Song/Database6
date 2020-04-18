#define DOMAIN EVAL
#include "eval.h"
#include <stdio.h>
#include "messenger.h"
#include "basic_linked_lists.h"
#include <stdbool.h>
#include <string.h>
DEFINE_LIST(Op)
DEFINE_LIST(Func)
DEFINE_LIST(Var)
DEFINE_LIST(Element)
const int type_value = 0;
const int type_name = 1;
const int type_operator = 2;
const int type_leftparn = 3;
const int type_rightparn = 4;
const int type_none = -1;

void element_free(Element e)
{
     free(e.literal);
}

Element element_copy(Element src)
{
     Element dest;
     dest.literal = string_duplicate(src.literal);
     dest.operator_kind = src.operator_kind;
     dest.type = src.type;
     return dest;
}

char opchars[] = "+-*/><=,|!%&";

int get_type(char ch)
{
     if (ch >= '0' && ch <= '9' || ch == '.' || ch == '\"' || ch == '-')
          return type_value;

     if (ch == '(')
          return type_leftparn;

     if (ch == ')')
          return type_rightparn;

     if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_')
          return type_name;

     for (int i = 0; i < strlen(opchars); i++)
     {
          if (opchars[i] == ch)
               return type_operator;
     }
     return type_none;
}

void log_elements(List(Element) * ls)
{
     static int id = 0;
     id++;
     log("Element Total:%d\n", list_count(Element)(ls));
     Element e;
     Foreach(Element, e, ls)
     {
          log("literal:%s type:%d kind:%d\n",  NS_LOG(e.literal), e.type, e.operator_kind);
     }
}

Calc calc_init(List(Op) * ops, List(Func) * funcs, List(Var) * vars)
{
     Calc calc;
     calc.functions = funcs;
     calc.operators = ops;
     calc.vars = vars;
     return calc;
}

Op initOp(const char *name, char *(*func)(const char *, const char *), int p, bool unary)
{
     Op op;
     op.func = func;
     op.name = name;
     op.priority = p;
     op.unary = unary;
     return op;
}

Func initFunc(const char *name, char *(*func)(const char *))
{
     Func f;
     f.func = func;
     f.name = name;
     return f;
}

Var initVar(const char *name, const char *value)
{
     Var v;
     v.name = name;
     v.value = value;
     return v;
}

char *eval_e(Calc calc, List(Element) * _elems, bool *error);
char *eval(Calc calc, const char *expr)
{
     if (!expr)
     {
          return NULL;
     }

     int expr_len = strlen(expr);
     List(Element) *elements = list_create(Element)(element_free);
     bool quoted = false;
     char *literal = (char *)calloc(sizeof(char), expr_len + 1);
     int literal_length = 0;
     for (int x = 0; x <= expr_len; x++)
     {
          char lastchar = (x == 0) ? 0 : expr[x - 1];
          char thischar = expr[x];
          char nextchar = x == expr_len ? 0 : expr[x + 1];

          if (thischar == '\"' && lastchar != '\\' && !quoted)
          {
               quoted = true;
               continue;
          }

          if (thischar == '\"' && lastchar != '\\' && quoted)
          {
               quoted = 0;
               Element newelement;
               newelement.literal = literal;
               newelement.type = type_value;
               list_append(Element)(elements, newelement);
               literal = (char *)calloc(sizeof(char), expr_len + 1);
               literal_length = 0;
               continue;
          }

          if (quoted)
          {
               literal[literal_length] = thischar;
               literal_length++;
          }
          else // not inside quote
          {
               literal[literal_length] = thischar;
               literal_length++;

               if (thischar == '(')
               {
                    Element newelement;
                    newelement.literal = literal;
                    newelement.type = type_leftparn;
                    list_append(Element)(elements, newelement);

                    literal = (char *)calloc(sizeof(char), expr_len + 1);
                    literal_length = 0;
                    continue;
               }

               if (thischar == ')')
               {
                    Element newelement;
                    newelement.literal = literal;
                    newelement.type = type_rightparn;
                    list_append(Element)(elements, newelement);

                    literal = (char *)calloc(sizeof(char), expr_len + 1);
                    literal_length = 0;
                    continue;
               }

               int thistype = get_type(thischar);
               int nexttype = get_type(nextchar);

               if (thistype != nexttype)
               {
                    Element newelement;
                    newelement.literal = literal;
                    newelement.type = thistype;
                    list_append(Element)(elements, newelement);

                    literal = (char *)calloc(sizeof(char), expr_len + 1);
                    literal_length = 0;
               }
          }
     }
     free(literal);
     bool error;
     log("MAIN EXPRESSION:\n");
     log_elements(elements);
     char *a = eval_e(calc, elements, &error);
     list_delete(Element)(elements);
     return a;
}

char *eval_e(Calc calc, List(Element) * _elems, bool *error)
{
     if (list_count(Element)(_elems) == 0)
          return NULL;

     // argelements deep copied to elements
     List(Element) *elements = list_create(Element)(element_free);
     Element e;
     Foreach(Element, e, _elems)
     {
          Element copied = e;
          copied.literal = string_duplicate(copied.literal);
          list_append(Element)(elements, copied);
     }
     log("SUB-EXPRESSION:\n");
     log_elements(elements);
     // remove parentheses recursively
     int *stack = calloc(sizeof(int), list_count(Element)(elements));
     int stackcnt = 0;
     int index = 0;
     int found_paren = 1;
     while (found_paren)
     {
          index = -1;
          found_paren = 0;
          Foreach(Element, e, elements)
          {
               index++;

               if (e.type == type_leftparn)
               {
                    stack[stackcnt] = index;
                    stackcnt++;
               }

               if (e.type == type_rightparn)
               {
                    int rightindex = index;
                    int leftindex = stack[stackcnt - 1];
                    stackcnt--;

                    if (stackcnt == 0)
                    {
                         // copy the in-parentheses part of elements to inner_ele
                         // and pass inner_e lements to the next recursion
                         List(Element) *inner_elements = list_create(Element)(element_free);

                         index = -1;
                         Element e1;
                         Foreach(Element, e1, elements)
                         {
                              index++;
                              if (index >= leftindex + 1 && index <= rightindex - 1)
                              {
                                   Element copied = e1;
                                   copied.literal = string_duplicate(copied.literal);
                                   list_append(Element)(inner_elements, copied);
                              }
                         }
                         bool has_error = false;
                         char *result = eval_e(calc, inner_elements, &has_error);
                         list_delete(Element)(inner_elements);

                         if (has_error)
                         {
                              *error = true;
                              list_delete(Element)(elements);
                              free(stack);
                              return NULL;
                         }

                         // remove left and right parens and insert result there.
                         list_remove(Element)(elements, leftindex, rightindex - leftindex + 1);
                         Element newelement;
                         newelement.literal = result;
                         newelement.type = type_value;
                         list_insert(Element)(elements, leftindex, newelement);

                         // make sure we break the foreach cuz we modified the list
                         found_paren = 1;
                         break;
                    }
                    // ERROR: unsymmetric parentheses
                    else if (stackcnt < 0)
                    {
                         warn("Unpaired parentheses.\n");
                         free(stack);
                         list_delete(Element)(elements);
                         *error = true;
                         return NULL;
                    }
               }
          }
          // ERROR: unsymmetric parentheses
          if (stackcnt != 0)
          {
               warn("Unpaired parentheses.\n");
               free(stack);
               list_delete(Element)(elements);
               *error = true;
               return NULL;
          }
     }
     free(stack);
     // reduce all functions and constants

     bool replace_done = true;
     while (replace_done)
     {
          index = -1;
          replace_done = 0;
          Foreach(element, e, elements)
          {
               index++;
               if (e.type == type_name)
               {
                    Var v;
                    Foreach(Var, v, calc.vars)
                    {
                         if (!strcmp(v.name, e.literal))
                         {
                              list_remove(Element)(elements, index, 1);
                              Element new_elem;
                              new_elem.literal = string_duplicate(v.value);
                              new_elem.type = type_value;
                              list_insert(Element)(elements, index, new_elem);
                              replace_done = 1;
                              break;
                         }
                    }

                    if (replace_done)
                         break;

                    Func fun;
                    Foreach(Func, fun, calc.functions)
                    {
                         if (!strcmp(fun.name, e.literal))
                         {
                              Element new_elem;
                              if (index + 1 >= list_count(Element)(elements))
                              {
                                   warn("Function cannot be used as a variable.\n");
                                   *error = true;
                                   list_delete(Element)(elements);
                                   return NULL;
                              }
                              Element next_elem = list_get(Element)(elements, index + 1);

                              // call the function and replace the result
                              if (next_elem.type == type_operator)
                              {
                                   warn("Function cannot be used as a variable.\n");
                                   *error = true;
                                   list_delete(Element)(elements);
                                   return NULL;
                              }
                              else
                              {
                                   new_elem.literal = fun.func(next_elem.literal);
                                   new_elem.type = type_value;
                                   list_remove(Element)(elements, index, 2);
                              }
                              list_insert(Element)(elements, index, new_elem);
                              replace_done = 1;
                              break;
                         }
                    }
                    if (replace_done)
                         break;
                    // ERROR: Wrong Name
                    warn("Unknown function or variable name.\n");
                    *error = true;
                    list_delete(Element)(elements);
                    return NULL;
               }
          }
     }
     // make unary operators binary
     // and check operators

     int list_changed = 1;
     while (list_changed)
     {
          index = -1;
          list_changed = 0;
          Foreach(element, e, elements)
          {
               index++;
               if (e.type == type_operator)
               {
                    int foundop = 0;
                    int op_index = -1;
                    Op op;
                    Foreach(Op, op, calc.operators)
                    {
                         op_index++;
                         if (!strcmp(op.name, e.literal))
                         {
                              foundop = 1;
                              list_get_node_pointer(Element)(elements, index)->data.operator_kind = op_index;
                              if (op.unary && (index == 0 || list_get(Element)(elements, index - 1).type == type_operator))
                              {
                                   Element new_element;
                                   new_element.literal = string_duplicate("0");
                                   new_element.type = type_value;
                                   list_insert(Element)(elements, index, new_element);
                                   list_changed = 1;
                              }
                              break;
                         }
                    }

                    if (!foundop)
                    {
                         warn("Unknown Operator.\n");
                         *error = true;
                         list_delete(Element)(elements);
                         return NULL;
                    }
                    if (list_changed)
                         break;
               }
          }
     }
     // check format
     if (list_count(Element)(elements) % 2 == 0) //"1+1*2" (right) "2+" (wrong)
     {
          warn("Bad format.\n");
          *error = true;
          list_delete(Element)(elements);
          return NULL;
     }
     int correct_type = type_value;
     index = -1;
     Foreach(Element, e, elements)
     {
          index++;
          if (e.type != correct_type)
          {
               warn("Bad format.\n");
               *error = true;
               list_delete(Element)(elements);
               return NULL;
          }
          correct_type = correct_type == type_value ? type_operator : type_value;
     }

     // calculate operators
     while (1)
     {
          if (list_count(Element)(elements) == 1) // only one element left, should be a value
          {
               Element only_element = list_get(Element)(elements, 0);

               if (only_element.type != type_value)
               {
                    // should not reach here under any circumstances.
                    warn("Bad format.\n");
                    *error = true;
                    list_delete(Element)(elements);
                    return NULL;
               }
               else
               {
                    char *result = string_duplicate(only_element.literal);
                    list_delete(Element)(elements);
                    return result; // EXIT POINT
               }
          }
          // multiple elements left
          int prioritized_op_pos = 1;
          Op prioritized_op;
          bool found_prioritized_op = false;
          index = -1;
          // find the operator with the highest(smallest in number) priority
          Foreach(element, e, elements)
          {
               index++;
               if (e.type == type_operator)
               {
                    Op this_op = list_get(Op)(calc.operators, e.operator_kind);
                    if (found_prioritized_op)
                    {
                         if (this_op.priority < prioritized_op.priority)
                         {
                              prioritized_op = this_op;
                              prioritized_op_pos = index;
                         }
                    }
                    else
                    {
                         prioritized_op = this_op;
                         prioritized_op_pos = index;
                         found_prioritized_op = true;
                    }
               }
          }
          char *lvalue = list_get(Element)(elements, prioritized_op_pos - 1).literal;
          char *rvalue = list_get(Element)(elements, prioritized_op_pos + 1).literal;
          char *op_result = prioritized_op.func(lvalue, rvalue);
          Element result_elem;
          result_elem.type = type_value;
          result_elem.literal = op_result;
          list_remove(Element)(elements, prioritized_op_pos, 1);
          list_insert(Element)(elements, prioritized_op_pos, result_elem);
          list_remove(Element)(elements, prioritized_op_pos + 1, 1);
          list_remove(Element)(elements, prioritized_op_pos - 1, 1);
     }
}