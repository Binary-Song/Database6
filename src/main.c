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
char *log_name;
char *this_file;
int main(int argc, char *argv[])
{
    this_file = argv[0]; 
    memcheck_begin();

    log_name = new (500); 
    time_t now; 
    struct tm *tm_now; 
    time(&now); 
    tm_now = localtime(&now); 
    string prefix = path_prefix(argv[0]);
    snprintf(log_name, 499, "%s/Runtime Log %d-%d-%d %d:%02d:%02d %s.log", prefix, tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, tm_now->tm_hour > 5 && tm_now->tm_hour < 17 ? "☀️" : "🌙");
    log("init\n");
    
    string temp_path =new(strlen(prefix)+10);
    strcpy(temp_path,prefix);
    strcat(temp_path,"/.temp");
     
    db_init(); 
    db_load_file(temp_path);
    exec(argc,argv);
    db_save_file(temp_path);
    db_deinit();

    delete(prefix);
    delete (log_name);
    return 0;
}