#define DOMAIN CMDANLS
//#define DISABLE_LOG
#include "cmd_analysis.h"
#include "basic_linked_lists.h"
#include "string.h"
#include "stdbool.h"
#include "messenger.h"
#include "stack.h"

DEFINE_LIST(Pair)
DEFINE_LIST(Tag)

string dict_cl[] = {
    "add-record" //
};
string dict_cp[] = {
    "add-field",     //
    "list-field",    //
    "list-record",   //
    "remove-record", //
    "update-field",  //
    "remove-field",  //
    "update-record", //
    "save",
    "load",
};
string dict_key[] = {
    "name",       //
    "format",     //
    "constr",     //
    "info",       //
    "index",      //
    "set-name",   //
    "set-constr", //
    "set-format", //
    "set-info",   //
    "set-unique", //
    "filter",     //
    "sort",       //
    "value",      //
    "field",
    "file",
};
string dict_tag[] = {
    "unique",         //
    "disable-unique", //
    "disable-format", //
    "disable-info",   //
    "disable-constr", //
    "raw",            //
    "detailed",
};

typedef void (*CMDFunc_Pair)(List(Pair) *, List(Tag) *);
typedef void (*CMDFunc_ArgList)(List(string) *);

typedef struct CMDInfo_ArgList
{
    string name;
    CMDFunc_ArgList func;
} CMDInfo_ArgList;

typedef struct CMDInfo_Pair
{
    string name;
    CMDFunc_Pair func;
} CMDInfo_Pair;

CMDInfo_ArgList cmdinfobank_arglist[] = {
    {"add-record", cmd_add_record}, //
};
CMDInfo_Pair cmdinfobank_pair[] =
    {
        {"add-field", cmd_add_field},          //
        {"list-field", cmd_list_fields},       //
        {"remove-record", cmd_remove_record},  //
        {"list-record", cmd_list_records},     //
        {"update-field", cmd_configure_field}, //
        {"remove-field", cmd_remove_field},    //
        {"update-record", cmd_update_record},  //
        {"save", cmd_save},                    //
        {"load", cmd_load},                    // 
};

#pragma region SCANNER THINGS 词法分析相关

typedef enum _WordType
{
    nWT_KEYWORD,
    nWT_ARGSTR,
    nWT_INT,
    //终类型，参数
    WT_ARG,
    //终类型，列表型指令名
    WT_CL,
    //终类型，键值对/标签型指令名
    WT_CP,
    //终类型，键
    WT_KEY,
    //终类型，标签
    WT_TAG,
    //终类型，空
    WT_EMPTY
} WordType;

typedef struct _Token
{
    WordType word_type;
    char *content;
} Token;

void token_dealloc(Token tok)
{
    delete (tok.content);
}

DECLARE_LIST(Token)
DEFINE_LIST(Token)

List(Token) * Scan(int argc, char *argv[]);

typedef enum _chartype
{
    CTYPE_ALPHA = 0,
    CTYPE_SPACE = 1,
    CTYPE_QUOTE = 2,
    CTYPE_BACHSLASH = 3,
    CTYPE_NUMBER = 4,
    CTYPE_END = 5,
    CTYPE_HYPHEN = 6,
} CharType;

CharType getchartype(char c)
{
    if (c == 0)
    {
        return CTYPE_END;
    }
    if (c == ' ' || c == '\n' || c == '\t' || c == '\r')
    {
        return CTYPE_SPACE;
    }
    if (c == '"')
    {
        return CTYPE_QUOTE;
    }
    if (c == '\\')
    {
        return CTYPE_BACHSLASH;
    }
    if (c >= '0' && c <= '9' || c == '.')
    {
        return CTYPE_NUMBER;
    }
    if (c == '-')
    {
        return CTYPE_HYPHEN;
    }

    return CTYPE_ALPHA;
}

void log_tokens(List(Token) * tokens);
inline void log_tokens(List(Token) * tokens)
{
#ifdef DEBUG
    Token tok;
    Foreach(Token, tok, tokens)
    {
        const_string type_str;
        switch (tok.word_type)
        {
        case nWT_KEYWORD:
            type_str = "keyword";
            break;
        case nWT_INT:
            type_str = "int";
            break;
        case nWT_ARGSTR:
            type_str = "string";
            break;
        case WT_ARG:
            type_str = "ARG";
            break;
        case WT_CL:
            type_str = "CL";
            break;
        case WT_CP:
            type_str = "CP";
            break;
        case WT_KEY:
            type_str = "KEY";
            break;
        case WT_TAG:
            type_str = "TAG";
            break;
        case WT_EMPTY:
            type_str = "EMPTY";
            break;
        default:
            type_str = "unknown";
            break;
        }
        log(" <TOKEN> " C_LOG_KEY "content" C_LOG_RESET ":" C_LOG_VALUE "%-25s" C_LOG_VALUE "\t" C_LOG_KEY "type" C_LOG_RESET ":" C_LOG_VALUE "%-8s" C_LOG_RESET "\n", tok.content, type_str);
    }
#endif
}

bool isInArray(string str, string *arr, int length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (!strcmp(arr[i], str))
            return true;
    }
    return false;
}

#pragma endregion

//词法分析
List(Token) * Scan(int argc, char *argv[])
{
    List(Token) *tokens = list_create(Token)(token_dealloc);
    Token newtk;
    for (size_t i = 1; i < argc; i++)
    {
        if (isInArray(argv[i], dict_cl, sizeof(dict_cl) / sizeof(string)))
        {
            newtk.word_type = WT_CL;
            newtk.content = string_duplicate(argv[i]);
        }
        else if (isInArray(argv[i], dict_cp, sizeof(dict_cp) / sizeof(string)))
        {
            newtk.word_type = WT_CP;
            newtk.content = string_duplicate(argv[i]);
        }
        else if (isInArray(argv[i], dict_key, sizeof(dict_key) / sizeof(string)))
        {
            newtk.word_type = WT_KEY;
            newtk.content = string_duplicate(argv[i]);
        }
        else if (isInArray(argv[i], dict_tag, sizeof(dict_tag) / sizeof(string)))
        {
            newtk.word_type = WT_TAG;
            newtk.content = string_duplicate(argv[i]);
        }
        else
        {
            newtk.word_type = WT_ARG;
            newtk.content = string_duplicate(argv[i]);
        }
        list_append(Token)(tokens, newtk);
    }

    return tokens;
}

#pragma region PARSER THINGS 语法分析相关
typedef enum _SymbolType
{
    ST_E,
    ST_L,
    ST_P,
    ST_T
} SymbolType;

typedef struct _Symbol
{
    SymbolType symtype;
    Token token;
} Symbol;

DECALRE_STACK(Symbol)

//DEFINE_STACK(Symbol)
#pragma region

StackSymbol *stack_createSymbol(void (*dealloc)(Symbol), Symbol (*copy)(Symbol))
{
    StackSymbol *stack = newmem(sizeof(StackSymbol), 1);
    stack->count = 0;
    stack->dealloc = dealloc;
    stack->copy = copy;
    stack->top = NULL;
    return stack;
}
StackNodeSymbol *stack_pushSymbol(StackSymbol *stack, Symbol data)
{
    if (!stack)
    {
        fprintf(stderr, "[ERROR](STACK): Unexpected null pointer.\n");
        exit(1);
    }
    StackNodeSymbol *node = newmem(sizeof(StackNodeSymbol), 1);
    node->data = data;
    node->inner = stack->top;
    stack->count++;
    stack->top = node;
    return node;
}
void stack_deleteSymbol(StackSymbol *stack)
{
    if (!stack)
    {
        fprintf(stderr, "[ERROR](STACK): Unexpected null pointer.\n");
        exit(1);
    }
    StackNodeSymbol *curr = stack->top;
    StackNodeSymbol *inner;
    while (curr)
    {
        inner = curr->inner;
        if (stack->dealloc)
        {
            stack->dealloc(curr->data);
        }
        delete (curr);
        curr = inner;
    }
    delete (stack);
}
Symbol stack_popSymbol(StackSymbol *stack)
{
    if (!stack || !stack->top)
    {
        fprintf(stderr, "[ERROR](LIST): Unexpected null pointer.\n");
        exit(1);
    }
    Symbol t = stack->copy(stack->top->data);
    stack->dealloc(stack->top->data);
    StackNodeSymbol *newtop = stack->top->inner;
    delete (stack->top);
    stack->top = newtop;
    stack->count--;
    return t;
}
Symbol stack_peekSymbol(StackSymbol *stack, int layer)
{
    StackNodeSymbol *node = stack->top;
    while (layer)
    {
        layer--;
        node = node->inner;
    }
    return node->data;
}
int stack_countSymbol(StackSymbol *stack) { return stack->count; }

#pragma endregion

void log_symbol_stack(Stack(Symbol) * symstac)
{
#ifdef DEBUG
    string type_str;
    for (size_t i = 0; i < symstac->count; i++)
    {
        Symbol s = stack_peek(Symbol)(symstac, i);
        if (s.symtype != ST_T)
        {
            switch (s.symtype)
            {
            case ST_L:
                type_str = "L";
                break;
            case ST_E:
                type_str = "E";
                break;
            case ST_P:
                type_str = "P";
                break;
            default:
                type_str = "Unknown";
                break;
            }
        }
        else
        {
            switch (s.token.word_type)
            {
            case WT_ARG:
                type_str = "T arg";
                break;
            case WT_CL:
                type_str = "T cl";
                break;
            case WT_CP:
                type_str = "T cp";
                break;
            case WT_KEY:
                type_str = "T key";
                break;
            case WT_TAG:
                type_str = "T tag";
                break;
            case WT_EMPTY:
                type_str = "T empty";
                break;
            default:
                type_str = "T unknown";
                break;
            }
        }
        log(" <SYMBOL> " C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(type_str));
    }
#endif
}

void symbol_dealloc(Symbol sym)
{
    token_dealloc(sym.token);
}

Symbol symbol_init(SymbolType type, WordType token_type, const_string token_content)
{
    Symbol newsym;
    newsym.symtype = type;
    newsym.token.word_type = token_type;
    newsym.token.content = string_duplicate(token_content);
    return newsym;
}

Symbol symbol_copy(Symbol sym)
{
    return symbol_init(sym.symtype, sym.token.word_type, sym.token.content);
}

Symbol symbol_init_nonterminal(SymbolType type)
{
    return symbol_init(type, 0, NULL);
}

Symbol symbol_init_terminal(WordType token_type, const_string token_content)
{
    return symbol_init(ST_T, token_type, token_content);
}

bool symbol_equal(Symbol sym1, Symbol sym2)
{
#ifdef DEBUG
    if (sym1.symtype != ST_T || sym2.symtype != ST_T)
        err("Non-terminals aren't supposed to be compared.\n");
#endif
    return sym1.token.word_type == sym2.token.word_type;
}

bool symbol_empty(Symbol sym)
{
    return sym.symtype == ST_T && sym.token.word_type == WT_EMPTY;
}

Pair Pair_init(string key, string value)
{
    Pair pair;
    pair.key = string_duplicate(key);
    pair.value = string_duplicate(value);
    return pair;
}

void Pair_dealloc(Pair pair)
{
    delete (pair.key);
    delete (pair.value);
}

Tag Tag_init(string tag)
{
    Tag t;
    t.value = string_duplicate(tag);
    return t;
}

void Tag_dealloc(Tag tag)
{
    delete (tag.value);
}

typedef struct _ParseResult
{
    enum CMDType
    {
        CMDType_ArgList,
        CMDType_Pair,
    } cmd_type;

    union {
        List(Pair) * pairs;
        List(string) * arglist;
    } Parameters;

    union {
        CMDInfo_Pair cmdinfo_pair;
        CMDInfo_ArgList cmdinfo_arglist;
    } CommandInfo;

    List(Tag) * tags;
} ParseResult;

void log_parse_result(ParseResult pr)
{
#ifdef DEBUG
    log("PARSE RESULT:\n");
    if (pr.cmd_type == CMDType_ArgList)
    {
        log("" C_LOG_KEY "Command" C_LOG_RESET ":" C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(pr.CommandInfo.cmdinfo_arglist.name));
        log("" C_LOG_KEY "Type" C_LOG_RESET ":" C_LOG_VALUE "ArgList" C_LOG_RESET "\n");
        log("" C_LOG_KEY "Args" C_LOG_RESET ":" C_LOG_RESET "\n");
        string argstr;
        Foreach(string, argstr, pr.Parameters.arglist)
        {
            log(C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(argstr));
        }
    }
    else
    {
        log("" C_LOG_KEY "Command" C_LOG_RESET ":" C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(pr.CommandInfo.cmdinfo_pair.name));
        log("" C_LOG_KEY "Type" C_LOG_RESET ":" C_LOG_VALUE "KeyValuePair" C_LOG_RESET "\n");
        log("" C_LOG_KEY "Pairs" C_LOG_RESET ":" C_LOG_RESET "\n");
        Pair pair;
        Foreach(string, pair, pr.Parameters.pairs)
        {
            log("  " C_LOG_KEY "%s" C_LOG_RESET "=" C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(pair.key), NS_LOG(pair.value));
        }
        log("" C_LOG_KEY "Tags" C_LOG_RESET ":" C_LOG_RESET "\n");
        Tag tag;
        Foreach(Tag, tag, pr.tags)
        {
            log("  " C_LOG_VALUE "%s" C_LOG_RESET "\n", NS_LOG(tag.value));
        }
    }
#endif
}

CMDInfo_ArgList *find_funcinfo_arglist(const_string name)
{
    for (size_t i = 0; i < sizeof(cmdinfobank_arglist) / sizeof(CMDInfo_ArgList); i++)
    {
        if (!strcmp(cmdinfobank_arglist[i].name, name))
        {
            return &cmdinfobank_arglist[i];
        }
    }
    return NULL;
}

CMDInfo_Pair *find_funcinfo_pair(const_string name)
{
    for (size_t i = 0; i < sizeof(cmdinfobank_pair) / sizeof(CMDInfo_Pair); i++)
    {
        if (!strcmp(cmdinfobank_pair[i].name, name))
        {
            return &cmdinfobank_pair[i];
        }
    }
    return NULL;
}

#pragma endregion

//语法分析
bool Parse(List(Token) * tokens, ParseResult *result)
{
    log("Parsing...\n");
    if (tokens->count == 0)
    {
        warn("Empty input.\n");
        return false;
    }
    //初始化两个栈： input和pattern
    Stack(Symbol) *inputstac = stack_create(Symbol)(symbol_dealloc, symbol_copy);
    Stack(Symbol) *pattstac = stack_create(Symbol)(symbol_dealloc, symbol_copy);

    //将input栈填充，倒序遍历tokens，一一入栈。这样顶部是最左侧的词。
    ListNode(Token) * iter;
    iter = tokens->head;
    while (iter->next)
    {
        iter = iter->next;
    }
    //用3个空词垫底，方便之后peek不会出错。
    stack_push(Symbol)(inputstac, symbol_init_terminal(WT_EMPTY, NULL));
    stack_push(Symbol)(inputstac, symbol_init_terminal(WT_EMPTY, NULL));
    stack_push(Symbol)(inputstac, symbol_init_terminal(WT_EMPTY, NULL));
    do
    {
        stack_push(Symbol)(inputstac, symbol_init_terminal(iter->data.word_type, iter->data.content));
        iter = iter->prev;
    } while (iter->prev);

    //pattern栈顶为E
    stack_push(Symbol)(pattstac, symbol_init_terminal(WT_EMPTY, NULL));
    stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_E));

    //迭代，直到耗尽input或者patt (任一为空，即跳出循环)
    while (!symbol_empty(stack_peek(Symbol)(inputstac, 0)) && !symbol_empty(stack_peek(Symbol)(pattstac, 0)))
    {
        log("INPUT STACK\n");
        log_symbol_stack(inputstac);

        log("PATTERN STACK\n");
        log_symbol_stack(pattstac);

        Symbol patt0 = stack_peek(Symbol)(pattstac, 0);   //pattern顶部
        Symbol input0 = stack_peek(Symbol)(inputstac, 0); // input顶部

        //如果pattern顶部是终结符，就比较
        if (patt0.symtype == ST_T)
        {
            //如果和input相等，就一起pop
            if (symbol_equal(patt0, input0))
            {
                log("" C_LOG_MSG "Same Terminal Symbols Poped!" C_LOG_RESET "\n");
                stack_pop(Symbol)(pattstac);
                stack_pop(Symbol)(inputstac);
            }
            else //不回溯，直接报错
            {
                warn("Parse error.\n");
                stack_delete(Symbol)(inputstac);
                stack_delete(Symbol)(pattstac);
                return false;
            }
        }
        else //pattern顶部是非终结符，就展开
        {
            //E
            if (patt0.symtype == ST_E)
            {
                //E -> cl L
                if (input0.token.word_type == WT_CL)
                {
                    stack_pop(Symbol)(pattstac);
                    stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_L));
                    stack_push(Symbol)(pattstac, symbol_init_terminal(WT_CL, NULL));

                    //检查指令存在
                    CMDInfo_ArgList *info_ptr = find_funcinfo_arglist(input0.token.content);
                    if (info_ptr) //设置result
                    {
                        result->cmd_type = CMDType_ArgList;
                        result->CommandInfo.cmdinfo_arglist = *info_ptr;
                        result->Parameters.arglist = list_create(string)(string_dealloc);
                        log("" C_LOG_MSG "Rule applied: E -> cl L" C_LOG_RESET "\n");
                    }
                    else
                    {
                        warn("Command Lost. Blame the developer.\n");
                        stack_delete(Symbol)(inputstac);
                        stack_delete(Symbol)(pattstac);
                        return false;
                    }
                }
                //E
                else if (input0.token.word_type == WT_CP)
                {
                    Symbol input1 = stack_peek(Symbol)(inputstac, 1);

                    //E -> cp
                    if (input1.token.word_type == WT_EMPTY)
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_CP, NULL));
                        log("" C_LOG_MSG "Rule applied: E -> cp" C_LOG_RESET "\n");
                    }
                    //E -> cp P
                    else
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_P));
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_CP, NULL));
                        log("" C_LOG_MSG "Rule applied: E -> cp P" C_LOG_RESET "\n");
                    }
                    //检查指令存在
                    CMDInfo_Pair *info = find_funcinfo_pair(input0.token.content);
                    if (info) //设置result
                    {
                        result->cmd_type = CMDType_Pair;
                        result->CommandInfo.cmdinfo_pair = *info;
                        result->Parameters.pairs = list_create(Pair)(Pair_dealloc);
                        result->tags = list_create(Tag)(Tag_dealloc);
                    }
                    else
                    {
                        warn("Command Lost. Blame the developer.\n");
                        stack_delete(Symbol)(inputstac);
                        stack_delete(Symbol)(pattstac);
                        return false;
                    }
                }
                else
                {
                    warn("Command name expected.\n");
                    stack_delete(Symbol)(inputstac);
                    stack_delete(Symbol)(pattstac);
                    return false;
                }
            }
            //L
            else if (patt0.symtype == ST_L)
            {
                if (input0.token.word_type == WT_ARG)
                {
                    Symbol input1 = stack_peek(Symbol)(inputstac, 1); //input深度1
                    //L -> a L
                    if (input1.token.word_type == WT_ARG)
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_L));
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_ARG, NULL));

                        //添加到result的参数列表
                        list_append(string)(result->Parameters.arglist, string_duplicate(input0.token.content));

                        log("" C_LOG_MSG "Rule applied: L -> a L" C_LOG_RESET "\n");
                    }
                    //L -> a
                    else
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_ARG, NULL));

                        //添加到result的参数列表
                        list_append(string)(result->Parameters.arglist, string_duplicate(input0.token.content));

                        log("" C_LOG_MSG "Rule applied: L -> a" C_LOG_RESET "\n");
                    }
                }
                else
                {
                    warn("Symbol \"%s\" is not allowed in an argument list.\n", NS_LOG(input0.token.content));
                    stack_delete(Symbol)(inputstac);
                    stack_delete(Symbol)(pattstac);
                    return false;
                }
            }
            else if (patt0.symtype == ST_P)
            {
                Symbol input1 = stack_peek(Symbol)(inputstac, 1); //input深度1
                Symbol input2 = stack_peek(Symbol)(inputstac, 2); //input深度2
                if (input0.token.word_type == WT_KEY)             //下一个是key
                {
                    if (input1.token.word_type == WT_ARG) //下下个是arg
                    {
                        //P -> k a P
                        if (input2.token.word_type == WT_KEY     //下下下个是key
                            || input2.token.word_type == WT_TAG) //或下下下个是tag
                        {
                            stack_pop(Symbol)(pattstac);
                            stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_P));
                            stack_push(Symbol)(pattstac, symbol_init_terminal(WT_ARG, NULL));
                            stack_push(Symbol)(pattstac, symbol_init_terminal(WT_KEY, NULL));

                            //添加到result的参数列表
                            list_append(Pair)(result->Parameters.pairs, Pair_init(input0.token.content, input1.token.content));

                            log("" C_LOG_MSG "Rule applied: P -> k a P" C_LOG_RESET "\n");
                        }
                        //P -> k a
                        else
                        {
                            stack_pop(Symbol)(pattstac);
                            stack_push(Symbol)(pattstac, symbol_init_terminal(WT_ARG, NULL));
                            stack_push(Symbol)(pattstac, symbol_init_terminal(WT_KEY, NULL));

                            //添加到result的参数列表
                            list_append(Pair)(result->Parameters.pairs, Pair_init(input0.token.content, input1.token.content));

                            log("" C_LOG_MSG "Rule applied: P -> k a" C_LOG_RESET "\n");
                        }
                    }
                    else //错误
                    {
                        warn("Token \"%s\" requires a subsequent parameter.\n", NS_LOG(input0.token.content));
                        stack_delete(Symbol)(inputstac);
                        stack_delete(Symbol)(pattstac);
                        return false;
                    }
                }
                else if (input0.token.word_type == WT_TAG) //下一个是tag
                {
                    //P -> t P
                    if (input1.token.word_type == WT_TAG || input1.token.word_type == WT_KEY) //下下个是tag
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_nonterminal(ST_P));
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_TAG, NULL));

                        //添加到result的tag列表
                        list_append(Tag)(result->tags, Tag_init(input0.token.content));

                        log("" C_LOG_MSG "Rule applied: P -> t P" C_LOG_RESET "\n");
                    }
                    //P -> t
                    else
                    {
                        stack_pop(Symbol)(pattstac);
                        stack_push(Symbol)(pattstac, symbol_init_terminal(WT_TAG, NULL));

                        //添加到result的tag列表
                        list_append(Tag)(result->tags, Tag_init(input0.token.content));

                        log("" C_LOG_MSG "Rule applied: P -> t" C_LOG_RESET "\n");
                    }
                }
                else
                {
                    warn("Symbol \"%s\" is not allowed.\n", NS_LOG(stack_peek(Symbol)(inputstac, 0).token.content));
                    stack_delete(Symbol)(inputstac);
                    stack_delete(Symbol)(pattstac);
                    return false;
                }
            }
            else
            {
                warn("Unknown Error.\n");
                stack_delete(Symbol)(inputstac);
                stack_delete(Symbol)(pattstac);
                return false;
            }
        }
    }
    //正常情况下，input和pattern必须同时为空。否则就是语法设计问题。
    if (!symbol_empty(stack_peek(Symbol)(inputstac, 0)) || !symbol_empty(stack_peek(Symbol)(pattstac, 0)))
    {
        warn("Parse error.\n");
        stack_delete(Symbol)(inputstac);
        stack_delete(Symbol)(pattstac);
        return false;
    }
    stack_delete(Symbol)(inputstac);
    stack_delete(Symbol)(pattstac);
    log("" C_LOG_MSG "Input Accepted!" C_LOG_RESET "\n");
    return true;
}

void exec(int argc, char *argv[])
{
    List(Token) *tokens = Scan(argc, argv);
    if (!tokens)
    {
        return;
    }
    ParseResult result;
    result.Parameters.arglist = NULL;
    result.Parameters.pairs = NULL;
    result.tags = NULL;
    result.cmd_type = 0;
    if (Parse(tokens, &result)) //分析成功
    {
        log_parse_result(result);
        //根据指令类型，调用指令
        if (result.cmd_type == CMDType_ArgList)
        {
            result.CommandInfo.cmdinfo_arglist.func(result.Parameters.arglist);
        }
        else
        {
            result.CommandInfo.cmdinfo_pair.func(result.Parameters.pairs, result.tags);
        }
    }
    list_delete(Token)(tokens);
}
