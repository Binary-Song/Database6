#ifndef DB_H

#define DB_H

#include "memcheck.h"
#include "stdbool.h"
#include "stdlib.h"
#include "basic_linked_lists.h"

void db_list_record(const char *filter, const char *sort,bool raw);

void db_init();

void db_deinit();

void db_add_table(const_string name, const_string info);

void db_delete_table(const_string name);

void db_change_active_table(const_string name);

size_t db_field_count();

void db_list_fields();

void db_add_field(const_string name, const_string constraint, const_string format, const_string info, bool unique);

void db_delete_field(const_string name);

void db_config_field(const_string field_name, bool set_name, const_string new_name,
                     bool set_constr, const_string new_constraint,
                     bool set_format, const_string new_format,
                     bool set_info, const_string new_info,
                     bool set_unique, bool new_unique);
size_t db_record_count();

const_string db_get_value(const_string fieldname, int index);

void db_add_record(List(string) * values);

void db_delete_record(int index);

void db_update_record(int record_index, const_string field_name, const_string new_value);

void db_list_all_records();

void db_update_record(int record_index, const_string field_name, const_string new_value);

#endif