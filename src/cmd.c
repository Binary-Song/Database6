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
    
    string name = NULL;
    string constr = NULL;
    string format = NULL;
    string info = NULL;
    bool unique = false;
    Pair pair;
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
            warn("Unknown keyword:%s\n", NS_LOG(pair.value));
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
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(pair.value));
            return;
        }
    }
    db_add_field(name, constr, format, info, unique);
}

void cmd_list_fields(List(Pair) * pairs, List(Tag) * tags)
{
    bool detail = false;
    Tag tag;
    Foreach(Tag, tag, tags)
    {
        if (!strcmp(tag.value, "detailed"))
        {
            detail = true;
        }
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(tag.value));
            return;
        }
    }
    db_list_fields(detail);
}

void cmd_list_records(List(Pair) * pairs, List(Tag) * tags)
{
    string filter = NULL;
    string sort = NULL;

    Pair pair;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "filter"))
        {
            filter = pair.value;
        }
        else if (!strcmp(pair.key, "sort"))
        {
            sort = pair.value;
        }
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(pair.value));
            return;
        }
    }

    bool raw = false;
    bool detailed = false;
    Tag tag;
    Foreach(Tag, tag, tags)
    {
        if (!strcmp(tag.value, "raw"))
        {
            raw = true;
        }
        else if (!strcmp(tag.value, "detailed"))
        {
            detailed = true;
        }
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(tag.value));
            return;
        }
    }
    db_list_record(filter, sort, raw,detailed);
}

void cmd_remove_record(List(Pair) * pairs, List(Tag) * tags)
{
    Pair pair;
    string filter = NULL;

    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "filter"))
        {
            filter = pair.value;
            break;
        }
    }
    db_delete_record(filter);
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
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(tag.value));
            return;
        }
    }

    Pair pair;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "name"))
        {
            fieldname = pair.value;
        }
        else if (!strcmp(pair.key, "set-constr"))
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
        else if (!strcmp(pair.key, "set-format"))
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
        else if (!strcmp(pair.key, "set-info"))
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
        else if (!strcmp(pair.key, "set-unique"))
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
        else if (!strcmp(pair.key, "set-name"))
        {
            name = pair.value;
            setname = true;
        }
        else
        {
            warn("Unknown keyword:%s\n", NS_LOG(pair.value));
            return;
        }
    }

    if (!fieldname)
    {
        warn("Target field not provided.\n");
        return;
    }

    db_config_field(fieldname, setname, name, setconstr, constr, setformat, format, setinfo, info, setunique, unique);
}

void cmd_remove_field(List(Pair) * pairs, List(Tag) * tags)
{
    Pair pair;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "name"))
        {
            db_delete_field(pair.value);
        }
    }
}

void cmd_update_record(List(Pair) * pairs, List(Tag) * tags)
{
    Pair pair;
    string filter = NULL;
    string field= NULL;
    string value= NULL;
    Foreach(Pair, pair, pairs)
    {
        if (!strcmp(pair.key, "filter"))
        {
            filter = pair.value;
        }
        else if (!strcmp(pair.key, "field"))
        {
            field = pair.value;
        }
        else if (!strcmp(pair.key, "value"))
        {
            value = pair.value;
        }
    }
    if (!field)
    {
        warn("Field required!\n");
        return;
    }
    if (!filter)
    {
        warn("Filter required!\n");
        return;
    }
    if (!value)
    {
        warn("Value required!\n");
        return;
    }
    db_update_record(filter, field, value);
}

void cmd_load(List(Pair) * pairs, List(Tag) * tags)
{
    Pair p;
    Foreach(Pair, p, pairs)
    {
        if (!strcmp(p.key, "file"))
        {
            db_load_file(p.value);
            return;
        }
    }
}

void cmd_save(List(Pair) * pairs, List(Tag) * tags)
{
    Pair p;
    Foreach(Pair, p, pairs)
    {
        if (!strcmp(p.key, "file"))
        {
            db_save_file(p.value);
            return;
        }
    }
}
