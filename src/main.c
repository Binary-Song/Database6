#define DOMAIN MAIN
#include "db.h"
#include "eval.h"
#include "cmd_analysis.h"
#include "eval.h"
#include "eval_lib.h"
#include "utf8.h"
#include "stdio.h"
#include "messenger.h"
#include "time.h"
#include "string.h"
char temp_path[] = ".\\.db_staged";
char *log_name;
char *this_file;
int main(int argc, char *argv[])
{ 
    // FILE *pipe = popen("chcp", "r");
    // if (!pipe)
    // {
    //     warn("Unable to open pipe.\n");
    //     pclose(pipe);
    //     return 1;
    // }
    // long code = 0;
    // fscanf(pipe, "Active code page: %ld", &code);
    // pclose(pipe);

    // if (code != 65001)
    // {
    //     printf("Setting code page to utf-8...\n");
    //     system("chcp 65001");
    //     system("cls");
    // }

    this_file = argv[0];
    memcheck_begin();

    log_name = new (500);

    string prefix = path_prefix(argv[0]);
    snprintf(log_name, 499, "Database 6 Runtime Log");

    log("init\n");

    if (argc >= 2 && !strcmp(argv[1], "clean"))
    {
        if (remove(temp_path) != 0)
        {
            warn("Unable to clean.\n");
        }
        delete (prefix);
        delete (log_name);
        return 1;
    }

    FILE *temp = fopen(temp_path, "r");
    if (!temp) //文件不存在
    {
        temp = fopen(temp_path, "w");
        if (!temp)
        {
            err("Unable to create temp file.\n");
        }
        fclose(temp);
    }
    else
    {
        fclose(temp);
    }

    db_init();
    db_load_file(temp_path);
    exec(argc, argv);
    db_save_file(temp_path);
    db_deinit();

    delete (prefix);
    delete (log_name);
    return 0;
}