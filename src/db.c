﻿#define DOMAIN DB
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "basic_linked_lists.h"
#include "messenger.h"
#include "uchar.h"
#include "eval.h"
#include "eval_lib.h"
#include "wchar.h"
#include "utf8.h"

#define SET_COLOR(c) printf(c);

#pragma region RECORD 记录
typedef struct _Record
{
    List(string) * values;
} Record;

DECLARE_LIST(Record);
DEFINE_LIST(Record);

//回收Record就是回收里面的字符串链表。
void _record_deinit(Record record)
{
    list_delete(string)(record.values);
}
#pragma endregion

#pragma region FIELD 字段
typedef struct _Field
{
    bool unique;
    string name;
    string constraint;
    string format;
    string info;
} Field;

DECLARE_LIST(Field);
DEFINE_LIST(Field);
//生成一个字段,拷贝所有给的字符串。
Field _field_init(const_string name, const_string constraint, const_string format, const_string info, bool unique)
{
    Field field;
    field.name = string_duplicate(name);
    field.unique = unique;
    field.constraint = string_duplicate(constraint);
    field.format = string_duplicate(format);
    field.info = string_duplicate(info);
    return field;
}
//回收字段，回收所有的字符串。
void _field_deinit(Field f)
{
    string_dealloc(f.name);
    string_dealloc(f.constraint);
    string_dealloc(f.format);
    string_dealloc(f.info);
}
#pragma endregion

#pragma region TABLE 表
typedef struct _Table
{
    string name;
    string info;
    List(Field) * fields;
    List(Record) * records;
} Table;
DECLARE_LIST(Table)
DEFINE_LIST(Table)

//生成一个含有一个ID字段的表。
Table _table_init(const_string name, const_string info)
{
    Table table;
    //初始化各种值
    table.name = string_duplicate(name);
    table.info = string_duplicate(info);
    table.fields = list_create(Field)(_field_deinit);
    table.records = list_create(Record)(_record_deinit);
    //添加ID字段
    list_append(Field)(table.fields, _field_init(
                                         /*Name:*/ "ID",
                                         /*Constraint:*/ NULL,
                                         /*Format:*/ NULL,
                                         /*Info:*/ NULL,
                                         /*Unique:*/ true));
    return table;
}
//回收表。
void _table_deinit(Table table)
{
    //回收各种值
    list_delete(Field)(table.fields);
    list_delete(Record)(table.records);
    string_dealloc(table.info);
    string_dealloc(table.name);
}
#pragma endregion

#pragma region STATIC VARS 静态变量
static List(Table) * tables;
static ListNode(Table) * active_table_node;
static bool initialized;
#pragma endregion

#pragma region PRIVARTE TOOLS 私有工具

#define RECORDS activ_records()
#define FIELDS activ_fields()
#define INFO activ_info()
#define NAME activ_name()

#define GET_RECORD(i) (list_get(Record)(RECORDS, i))
#define GET_FIELD(i) (list_get(Field)(FIELDS, i))
#define GET_VALUE(f, r) (_get_value_by_index(f, r))
List(Record) * activ_records()
{
    return active_table_node->data.records;
}
List(Field) * activ_fields()
{
    return active_table_node->data.fields;
}
const_string activ_info()
{
    return active_table_node->data.info;
}
const_string activ_name()
{
    return active_table_node->data.name;
}

// 按index取得数据库的值
const_string _get_value_by_index(int field_index, int record_index)
{
    return list_get(string)(list_get(Record)(RECORDS, record_index).values, field_index);
}

// 按index检查值是否唯一
bool _check_unique(int field_index, int record_index, int *another)
{
    Field target_field = list_get(Field)(FIELDS, field_index);
    if (target_field.unique)
    {
        //将target_value与同字段值一一比照
        const_string target_value = GET_VALUE(field_index, record_index);

        Record r;
        int i = -1;
        Foreach(Record, r, RECORDS)
        {
            ++i;
            if (!strcmp(list_get(string)(r.values, field_index), target_value) && i != record_index)
            {
                *another = i;
                return false;
            }
        }
    }
    return true;
}

// 按index检查是否满足constraint。未完成
bool _check_constraint(int field_index, int record_index)
{
    Field field = GET_FIELD(field_index);

    if (field.constraint)
    {
        Var accessor;
        accessor.name = "value";
        accessor.value = GET_VALUE(field_index, record_index);

        Calc calc = init_std_calc();
        list_append(Var)(calc.vars, accessor);

        char *expr = newmem(1, strlen(field.constraint) + 10);
        sprintf(expr, "(%s)!=0", field.constraint);

        char *eval_result = eval(calc, expr);

        if (!eval_result || !strcmp(eval_result, "0"))
        {
            deinit_calc(calc);
            delete(expr);
            return false;
        }
        delete(expr);
        deinit_calc(calc);
        return true;
    }

    return true;
}
typedef struct _CheckFieldInfo
{
    bool pass;
    enum
    {
        REASON_NONE,
        REASON_INVALID_FIELD_NAME,
        REASON_SAME_FIELD_NAME,
        REASON_INFO_CONSTR,
        REASON_INFO_UNIQUE,
    } reason;
    int index;
    int another_index;
} CheckFieldInfo;

// 按field_index检查字段合法性，包括字段名重复，字段约束和唯一性
CheckFieldInfo _check_field(int field_index)
{
    //检查字段名重复
    string name = GET_FIELD(field_index).name;

    if (!name)
    {
        CheckFieldInfo res = {false, REASON_INVALID_FIELD_NAME, field_index};
        return res;
    }

    Field fld;
    int i = -1;
    Foreach(Field, fld, FIELDS)
    {
        i++;
        if (!strcmp(name, fld.name) && i != field_index)
        {
            CheckFieldInfo res = {false, REASON_SAME_FIELD_NAME, i};
            return res;
        }
    }

    //检查字段约束和唯一性
    for (size_t i = 0; i < db_record_count(); i++)
    {
        if (!_check_constraint(field_index, i))
        {
            CheckFieldInfo res = {false, REASON_INFO_CONSTR, i};
            return res;
        }
        int another = 0;
        if (!_check_unique(field_index, i, &another))
        {
            CheckFieldInfo res = {false, REASON_INFO_UNIQUE, i, another};
            return res;
        }
    }
    CheckFieldInfo res = {true, REASON_NONE, 0};
    return res;
}
#pragma endregion

#pragma region DATABASE INTERFACE 数据库接口

#pragma region INIT AND DEINIT 初始化和逆初始化
//初始化数据库，生成一个默认表。并且将默认表设置为活动表。
void db_init()
{
    if (initialized)
    {
        warn("Database already initialized.\n");
        return;
    }
    tables = list_create(Table)(_table_deinit);
    //创建初始默认表，添加到全局变量tables里。
    //并设为活动表。
    active_table_node = list_append(Table)(tables, _table_init(
                                                       /*Name:*/ "Default Table",
                                                       /*Info:*/ "The default table, initialized with the database."));
    initialized = true;
}

//回收数据库的表占用的内存。并且把活动表设为NULL。
void db_deinit()
{
    if (!initialized)
    {
        warn("Database not initialized.\n");
        return;
    }
    list_delete(Table)(tables);
    initialized = false;
    active_table_node = NULL;
}
#pragma endregion

#pragma region TABLE CONTROLS 表控制
//向当前工程添加含有ID字段的表。名称不允许与已有表重复。
void db_add_table(const_string name, const_string info)
{
    //检查名称
    Table tab;
    Foreach(Table, tab, tables)
    {
        if (!strcmp(name, tab.name))
        {
            warn("Conflicting table name:\"%s\"\n", NS_LOG(name));
            return;
        }
    }
    log("Table added: %s (info:%s)\n", NS_LOG(name), NS_LOG(info));
    list_append(Table)(tables, _table_init(name, info));
    return;
}

//删除当前工程中某个名字的表。
void db_delete_table(const_string name)
{
    int i = -1;
    Table tab;
    Foreach(Table, tab, tables)
    {
        i++;
        if (!strcmp(name, tab.name))
        {
            list_remove(Table)(tables, i, 1);
            log("Table removed: %s\n", NS_LOG(name));
            return;
        }
    }
    warn("Table not found.\n");
    return;
}

//更改当前活动的表
void db_change_active_table(const_string name)
{
    int i = -1;
    Table tab;
    Foreach(Table, tab, tables)
    {
        i++;
        if (!strcmp(name, tab.name))
        {
            active_table_node = list_get_node_pointer(Table)(tables, i);
            log("Active table changed to %s\n", NS_LOG(name));
            return;
        }
    }
    warn("Table not found.\n");
    return;
}
#pragma endregion

#pragma region FIELD CONTROLS 字段控制
//取得字段数
size_t db_field_count()
{
    return list_count(Field)(active_table_node->data.fields);
}

void db_add_field(const_string name, const_string constraint, const_string format, const_string info, bool unique)
{
    if (!name || !strcmp(name, ""))
    {
        warn("A non-empty name is required!\n");
        return;
    }

    Field fld;
    int i = -1;
    //检查是否与其他字段名重复
    Foreach(Field, fld, FIELDS)
    {
        i++;
        if (!strcmp(fld.name, name))
        {
            warn("Conflicting field name:\"%s\"\n", NS_LOG(name));
            return;
        }
    }
    //添加字段
    Field newfld = _field_init(name, constraint, format, info, unique);

    log("Field added:%s (constr:%s format:%s info:%s unique:%d)\n",
        NS_LOG(name), NS_LOG(constraint), NS_LOG(format), NS_LOG(info), unique);
    list_append(Field)(FIELDS, newfld);

    //保证方块性
    Record rec;
    Foreach(Record, rec, RECORDS)
    {
        list_append(string)(rec.values, NULL);
    }
}

void db_delete_field(const_string name)
{
    if (!name || !strcmp(name, ""))
    {
        warn("A non-empty name is required!\n");
        return;
    }
    Field fld;
    int i = -1;
    //找到字段
    Foreach(Field, fld, FIELDS)
    {
        i++;
        if (!strcmp(fld.name, name))
        {
            //删字段
            list_remove(Field)(FIELDS, i, 1);
            Record rec;
            //删数据
            Foreach(Record, rec, RECORDS)
            {
                list_remove(string)(rec.values, i, 1);
            }
            return;
            log("Field deleted:%s\n", NS_LOG(name));
        }
    }
    warn("Field not found.\n");
    return;
}

void db_config_field(const_string field_name,                      //
                     bool set_name, const_string new_name,         //
                     bool set_constr, const_string new_constraint, //
                     bool set_format, const_string new_format,     //
                     bool set_info, const_string new_info,         //
                     bool set_unique, bool new_unique)             //
{
    if (!field_name || !strcmp(field_name, ""))
    {
        warn("A non-empty name is required!\n");
        return;
    }
    if (set_name && (!new_name || !strcmp(new_name, "")))
    {
        warn("Cannot set field name to null!\n");
        return;
    }
    //搜索目标字段 target
    Field fld;
    Field target;
    int i = -1;
    int target_i = -1;
    Foreach(Field, fld, FIELDS)
    {
        i++;
        if (!strcmp(fld.name, field_name))
        {
            target_i = i;
            target = fld;
            log("Config target found.\n");
            break;
        }
    }
    if (target_i == -1)
    {
        warn("Field not found.\n");
        return;
    }
    //复制target到oldfld
    Field oldfld = _field_init(target.name, target.constraint, target.format, target.info, target.unique);

    //根据参数产生newfld
    Field newfld = _field_init(set_name /*****/ ? new_name /********/ : oldfld.name,
                               set_constr /***/ ? new_constraint /**/ : oldfld.constraint,
                               set_format /***/ ? new_format /******/ : oldfld.format,
                               set_info /*****/ ? new_info /********/ : oldfld.info,
                               set_unique /***/ ? new_unique /******/ : oldfld.unique);

    list_set(Field)(FIELDS, target_i, newfld); //set 同时会调用delete!
    CheckFieldInfo check_res = _check_field(target_i);
    if (check_res.pass)
    {
        _field_deinit(oldfld);
    }
    else
    {
        if (check_res.reason == REASON_INVALID_FIELD_NAME)
        {
            warn("Unable to configure Field " C_FIELD "%s" C_WARNING ". New name \"%s\" invalid.\n", NS_LOG(field_name), NS_LOG(new_name));
        }
        else if (check_res.reason == REASON_SAME_FIELD_NAME)
        {
            warn("Unable to configure Field " C_FIELD "%s" C_WARNING ". "
                 "A field named \"%s\" already exists.\n",
                 NS_LOG(field_name), NS_LOG(new_name));
        }
        else if (check_res.reason == REASON_INFO_CONSTR)
        {
            warn("Unable to configure Field " C_FIELD "%s" C_WARNING ". "
                 "New constraint \"%s\" failed at record %d, value:\"%s\".\n",
                 NS_LOG(field_name), NS_LOG(new_constraint), check_res.index, NS_LOG(GET_VALUE(target_i, check_res.index)));
        }
        else if (check_res.reason == REASON_INFO_UNIQUE)
        {
            warn("Unable to configure Field " C_FIELD "%s" C_WARNING ". "
                 "Values in field are not unique as configured. Colliding records %d,%d, value:%s\n",
                 NS_LOG(field_name), check_res.index, check_res.another_index, NS_LOG(GET_VALUE(target_i, check_res.index)));
        }

        list_set(Field)(FIELDS, target_i, oldfld); //set 同时会调用delete!
    }
}

void db_list_fields()
{
    Field field;
    printf("Field Total:%d\n", list_count(Field)(FIELDS));
    int index = -1;
    Foreach(Field, field, FIELDS)
    {
        index++;
        printf("----------Field:%d----------\n", index);
        printf("Name:" C_FIELD "%s\n" C_RESET, NS(field.name));
        printf("Info:" C_VALUE "%s\n" C_RESET, NS(field.info));
        printf("Constraint:" C_VALUE "%s\n" C_RESET, NS(field.constraint));
        printf("Format:" C_VALUE "%s\n" C_RESET, NS(field.format));
        printf("Unique:" C_VALUE "%s\n" C_RESET, field.unique ? "true" : "false");
    }
}
#pragma endregion

#pragma region RECORD CONTROLS 记录控制
//取得记录数
size_t db_record_count()
{
    return list_count(Record)(RECORDS);
}

string format_record(string input, string format)
{
    Calc c = init_std_calc();

    Var v;
    v.name = "value";
    v.value = input;

    list_append(Var)(c.vars, v);

    char *res = eval(c, format);

    deinit_calc(c);

    return res;
}

//取得数据库的值。
const_string db_get_value(const_string field_name, int index)
{
    if (!field_name || !strcmp(field_name, ""))
    {
        warn("A non-empty name is required!\n");
        return NULL;
    }
    //找到字段
    int i = -1;
    int field_i = -1;
    Field fld;
    Foreach(Field, fld, FIELDS)
    {
        ++i;
        if (!strcmp(fld.name, field_name))
        {
            field_i = i;
            break;
        }
    }
    if (field_i == -1)
    {
        warn("Field not found.\n");
        return NULL;
    }
    return list_get(string)(list_get(Record)(RECORDS, index).values, field_i);
}

//添加记录
void db_add_record(List(string) * values)
{
    Record new_record;
    new_record.values = list_create(string)(string_dealloc);
    int t = 0; //t是添加的次数,必须正好等于 field_count。
    string val;
    Foreach(string, val, values)
    {
        if (t < db_field_count())
        {
            list_append(string)(new_record.values, string_duplicate(val));
            t++;
        }
        else
        {
            break;
        }
    }
    while (t < db_field_count())
    {
        list_append(string)(new_record.values, NULL);
        t++;
    }
    list_append(Record)(RECORDS, new_record);
    //添加了以后再检查
    for (size_t fi = 0; fi < db_field_count(); fi++)
    {

        int another = 0;
        if (!_check_constraint(fi, db_record_count() - 1))
        {
            //不行就删掉
            warn("Invalid record. "
                 "Field " C_FIELD "%s" C_WARNING " 's constraint " C_LOG_VALUE "%s" C_WARNING " cannot be satisfied "
                 "by value " C_LOG_VALUE "%s" C_WARNING ".\n",
                 NS_LOG(GET_FIELD(fi).name), NS_LOG(GET_FIELD(fi).constraint), NS_LOG(GET_VALUE(fi, db_record_count() - 1)));
            list_remove(Record)(RECORDS, db_record_count() - 1, 1);
            return;
        }
        else if (!_check_unique(fi, db_record_count() - 1, &another))
        {
            //不行就删掉
            warn("Invalid record. "
                 "Values in Field " C_FIELD "%s" C_WARNING " should be unique. But the given value " C_LOG_VALUE "%s" C_WARNING " already exists in record %d.\n",
                 NS_LOG(GET_FIELD(fi).name), NS_LOG(GET_VALUE(fi, db_record_count() - 1)), another);
            list_remove(Record)(RECORDS, db_record_count() - 1, 1);
            return;
        }
    }
    return;
}

//删除记录
void db_delete_record(int index)
{
    list_remove(Record)(RECORDS, index, 1);
}

void db_update_record(int record_index, const_string field_name, const_string new_value)
{
    Field fld;
    int fi = -1;
    //找到字段
    Foreach(Field, fld, FIELDS)
    {
        fi++;
        if (!strcmp(fld.name, field_name))
        {
            string oldvalue = string_duplicate(_get_value_by_index(fi, record_index));
            Record record = list_get(Record)(RECORDS, record_index);
            list_set(string)(record.values, fi, string_duplicate(new_value));

            int another = 0;
            if (!_check_constraint(fi, db_record_count() - 1))
            {

                warn("Invalid record. "
                     "Field " C_FIELD "%s" C_WARNING " 's constraint " C_LOG_VALUE "%s" C_WARNING " cannot be satisfied "
                     "by value " C_LOG_VALUE "%s" C_WARNING ".\n",
                     NS_LOG(GET_FIELD(fi).name), NS_LOG(GET_FIELD(fi).constraint), NS_LOG(GET_VALUE(fi, record_index)));

                list_set(string)(record.values, fi, oldvalue);
                return;
            }
            else if (!_check_unique(fi, db_record_count() - 1, &another))
            {

                warn("Invalid record. "
                     "Values in Field " C_FIELD "%s" C_WARNING " should be unique. But the given value " C_LOG_VALUE "%s" C_WARNING " already exists in record %d.\n",
                     NS_LOG(GET_FIELD(fi).name), NS_LOG(GET_VALUE(fi, record_index)), another);

                list_set(string)(record.values, fi, oldvalue);
                return;
            }

            return;
        }
    }
}

void db_list_all_records()
{
    Field f;
    Foreach(Field, f, FIELDS)
    {
        SET_COLOR(C_FIELD);
        fixed_print(f.name, 10, false);
        SET_COLOR(C_RESET);
        printf(" ");
    }  
    printf("\n");
    Record r;
    Foreach(Record, r, RECORDS)
    {
        string v;
        Foreach(string, v, r.values)
        {
            SET_COLOR(C_RECORD);
            fixed_print((v), 10, false); //fixed print 自己能处理null
            SET_COLOR(C_RESET);
            printf(" ");
        }
        printf("\n");
    }
}

static int record_index_context;

char *func_value(const char *a)
{
    if (!a || !strlen(a))
        return NULL;
    Field f;
    int i = -1;
    Foreach(Field, f, FIELDS)
    {
        i++;
        if (!strcmp(f.name, a))
        {
            return string_duplicate(GET_VALUE(i, record_index_context));
        }
    }
    return NULL;
}

void db_list_record(const char *filter, const char *sort)
{
    // List(int) *indices = list_create(int)(NULL);

    // Calc calc = init_std_calc();
    // Func func;
    // func.name = "value";
    // func.func = func_value;
    // list_append(Func)(calc.functions, func);
    // char *expr = newmem(strlen(filter) + 11, 1);
    // sprintf(expr, "(%s)!=0", filter);

    // Record r;
    // record_index_context = 0;
    // Foreach(Record, r, RECORDS)
    // {
    //     record_index_context++;
    //     char *res = eval(calc, expr);
    //     if (strcmp(res, "0")) //结果不为0,就认可
    //     {
    //         list_append(int)(indices, record_index_context);
    //     }
    //     delete(res);
    // }

    // list_delete(int)(indices);
    // delete(expr);
    // deinit_calc(calc);
}
#pragma endregion

#pragma endregion