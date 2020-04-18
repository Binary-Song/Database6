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
char *log_name;

int main()
{
    log_name = calloc(1, 50);

    time_t now;

    struct tm *tm_now;

    time(&now);

    tm_now = localtime(&now);

    snprintf(log_name, 49, "logs/Runtime Log %d-%d-%d %d:%02d:%02d %s.log", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, tm_now->tm_hour > 5 && tm_now->tm_hour < 17 ? "☀️" : "🌙");

    db_init();

    while (1)
    {
        msg("Awaiting Command\n");
        printf(">");
        char *input = calloc(1, 500);
        fgets(input, 499, stdin);
        log("INPUT COMMAND:%s\n", input);
        exec(input);
        free(input);
    }

    //     //exec("add-field format \"no\\\"format\" constr \"noconstr\" unique ");
    //     // printf(">>>\n");
    //     db_init();

    //     List(string) *vals = list_create(string)(NULL);
    //     list_append(string)(vals, "1");
    //     list_append(string)(vals, "1");
    //     db_add_field("FieldA", "value>0", NULL, "nothin", false);
    //     db_add_record(vals);

    //     db_list_all_records();

    //     db_config_field("FieldA",false,NULL,false,NULL,false,NULL,false,NULL,false,false);
    //     db_add_record(vals);
    //    // list_set(string)(vals,0,"-1");

    //    // db_add_record(vals);

    //    // db_list_all_records();

    //     db_deinit();
    free(log_name);
    return 0;
}