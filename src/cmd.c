#define DOMAIN CMD

#include "cmd.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include "messenger.h"
#include "db.h"
#include "str_utils.h"

void cmd_add_record(List(string) * a)
{
    db_add_record(a);
}
void cmd_add_field(List(Pair) * pairs, List(Tag) * tags)
{
    Pair pair;
    string name = NULL;
    string constr = NULL;
    string format = NULL;
    string info = NULL;
    bool unique = false;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "name"))
        {
            name = pair.value;
        }
        else if (!strcmp(pair.key, "constr"))
        {
            constr = pair.value;
        }
        else if (!strcmp(pair.key, "format"))
        {
            format = pair.value;
        }
        else if (!strcmp(pair.key, "info"))
        {
            info = pair.value;
        }
        else
        {
            warn("Unknown keyword.\n");
            return;
        }
    }
    Tag tag;
    Foreach(Tag, tag, tags)
    {
        if (!strcmp(tag.value, "unique"))
        {
            unique = true;
        }
        else if (!strcmp(tag.value, "disable-unique"))
            ;
        else if (!strcmp(pair.key, "disable-constr"))
            ;
        else if (!strcmp(pair.key, "disable-format"))
            ;
        else if (!strcmp(pair.key, "disable-info"))
            ;
        else
        {
            warn("Unknown tag.\n");
            return;
        }
    }
    db_add_field(name, constr, format, info, unique);
}

void cmd_list_fields(List(Pair) * pairs, List(Tag) * tags)
{
    db_list_fields();
}

void cmd_list_records(List(Pair) * pairs, List(Tag) * tags)
{
    db_list_all_records();
}

void cmd_remove_record(List(Pair) * pairs, List(Tag) * tags)
{
    Pair pair;
    Tag tag;
    bool really = false;
    Foreach(Tag, tag, tags)
    {
        if (!strcmp(tag.value, "really"))
        {
            really = true;
        }
        else
        {
            warn("Unknown tag:%s\n", tag.value);
            return;
        }
    }
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "index"))
        {
            char *p;
            long i = strtol(pair.value, &p, 10);
            if (*p) //转换失败
            {
                warn("Need an integer after keyword \"index\"!\n");
                return;
            }
            if (!really)
            {
                warn("Do you really want to delete this record?(y/n)\n");
                char ans[5] = {};
                fgets(ans, 4, stdin);
                if (ans[0] == 'y' || ans[0] == 'Y')
                {
                    db_delete_record(i);
                    logmsg("Record Deleted.\n");
                }
                else
                {
                    logmsg("Cancelled.\n");
                }
            }
            else
            {
                db_delete_record(i);
                logmsg("Record Deleted.\n");
            }

            return;
        }
    }
}

void cmd_configure_field(List(Pair) * pairs, List(Tag) * tags)
{
    Tag tag;
    string fieldname = NULL;

    bool setconstr = false;
    string constr = NULL;

    bool setformat = false;
    string format = NULL;

    bool setinfo = false;
    string info = NULL;

    bool setname = false;
    string name = NULL;

    bool setunique = false;
    bool unique = false;

    Foreach(Tag, tag, tags)
    {
        if (!strcmp(tag.value, "disable-constr"))
        {
            setconstr = true;
            constr = NULL;
        }
        else if (!strcmp(tag.value, "disable-format"))
        {
            setformat = true;
            format = NULL;
        }
        else if (!strcmp(tag.value, "disable-info"))
        {
            setinfo = true;
            info = NULL;
        }
        else if (!strcmp(tag.value, "disable-unique"))
        {
            setunique = true;
            unique = NULL;
        }
    }

    Pair pair;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "name"))
        {
            fieldname = pair.value;
        }
        if (!strcmp(pair.key, "set-constr"))
        {
            if (!setconstr)
            {
                setconstr = true;
                constr = pair.value;
            }
            else
            {
                warn("Self-contradictory request.\n");
                return;
            }
        }
        if (!strcmp(pair.key, "set-format"))
        {
            if (!setformat)
            {
                setformat = true;
                format = pair.value;
            }
            else
            {
                warn("Self-contradictory request.\n");
                return;
            }
        }
        if (!strcmp(pair.key, "set-info"))
        {
            if (!setinfo)
            {
                setinfo = true;
                info = pair.value;
            }
            else
            {
                warn("Self-contradictory request.\n");
                return;
            }
        }
        if (!strcmp(pair.key, "set-unique"))
        {
            if (!setunique)
            {
                setunique = true;
                bool success = true;
                unique = string_to_bool(pair.value, &success);
                if (!success)
                {
                    warn("The value after set-unique need to be either \"true\" or \"false\".\n");
                    return;
                }
            }
            else
            {
                warn("Self-contradictory request.\n");
                return;
            }
        }
        if (!strcmp(pair.key, "set-name"))
        {
            name = pair.value;
            setname = true;
        }
    }

    if (!fieldname   )
    {
        warn("Target field not provided.\n");
        return;
    }

    db_config_field(fieldname,setname,name,setconstr,constr,setformat,format,setinfo,info,setunique,unique);
}