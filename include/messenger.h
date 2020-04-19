#ifndef DEBUG_H
#define DEBUG_H
#include "memcheck.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

#define LOG_TO_FILE //可以关闭，这样log直接打到公屏上

#define C_WARNING "\e[033m"
#define C_ERROR "\e[031m"
#define C_DOMAIN "\e[036m"
#define C_RESET "\e[0m"
#define C_RECORD "\e[037m"
#define C_FIELD "\e[034m"
#define C_VALUE "\e[036m"
#define C_KEY "\e[032m"
#define C_NULL "\e[031m"
#define C_MSG "\e[032m"
#define C_RAW "\e[035m"

#ifdef LOG_TO_FILE

#define C_LOG_NULL ""
#define C_LOG_RESET ""
#define C_LOG_MSG ""
#define C_LOG_VALUE ""
#define C_LOG_KEY ""
#define C_LOG ""

#else

#define C_LOG_NULL "\e[031m"
#define C_LOG_RESET "\e[0m"
#define C_LOG_MSG "\e[037"
#define C_LOG_VALUE "\e[036m"
#define C_LOG_KEY "\e[036m"
#define C_LOG "\e[032m"

#endif

#define DEFINE_DOMAIN(D)     \
    _DEFINE_LOG_FUNCS(D)     \
    _DEFINE_WARNING_FUNCS(D) \
    _DEFINE_ERROR_FUNCS(D)   \
    _DEFINE_MESSAGE_FUNCS(D)

extern char *log_name;

#ifndef DISABLE_LOG

#ifdef LOG_TO_FILE
//enable log && log to file
#define _DEFINE_LOG_FUNCS(D)                                  \
    static void log##D(const char *format, ...);              \
    static void vlog##D(const char *format, va_list v);       \
    static inline void log##D(const char *format, ...)        \
    {                                                         \
        va_list v;                                            \
        va_start(v, format);                                  \
        vlog##D(format, v);                                   \
        va_end(v);                                            \
    }                                                         \
    static inline void vlog##D(const char *format, va_list v) \
    {                                                         \
        FILE *fp = fopen(log_name, "a");                      \
                                                              \
        fprintf(fp, "[LOG](" #D "):");                        \
        vfprintf(fp, format, v);                              \
                                                              \
        fclose(fp);                                           \
    }

#else //enable log && log to stdout
#define _DEFINE_LOG_FUNCS(D)                                                                           \
    static void log##D(const char *format, ...);                                                       \
    static void vlog##D(const char *format, va_list v);                                                \
     static inline void log##D(const char *format, ...)                                               \
    {                                                                                                  \
        fprintf(stderr, "[" C_LOG "LOG" C_LOG_RESET "](" C_DOMAIN "" #D "" C_LOG_RESET "):" C_LOG ""); \
        va_list v;                                                                                     \
        va_start(v, format);                                                                           \
        vprintf(format, v);                                                                            \
        va_end(v);                                                                                     \
        fprintf(stderr, "" C_LOG_RESET "");                                                            \
    }                                                                                                  \
    static inline void vlog##D(const char *format, va_list v)                                          \
    {                                                                                                  \
        fprintf(stdout, "[LOG](" #D "):");                                                             \
        vfprintf(stdout, format, v);                                                                   \
    }

#endif

#else //disable log

#define _DEFINE_LOG_FUNCS(D)                                  \
    static void log##D(const char *format, ...);              \
    static void vlog##D(const char *format, va_list v);       \
    static inline void log##D(const char *format, ...)        \
    {                                                         \
    }                                                         \
    static inline void vlog##D(const char *format, va_list v) \
    {                                                         \
    }

#endif

#pragma region WARNINGS
#ifdef LOG_TO_FILE
#define _DEFINE_WARNING_FUNCS(D)                                                                           \
    static void warn##D(const char *format, ...);                                                          \
    static inline void warn##D(const char *format, ...)                                                    \
    {                                                                                                      \
        FILE *fp = fopen(log_name, "a");                                                                   \
        va_list v;                                                                                         \
        va_start(v, format);                                                                               \
        fprintf(fp, "[WARNING](" #D "):");                                                                 \
        vfprintf(fp, format, v);                                                                           \
                                                                                                           \
        va_end(v);                                                                                         \
        fclose(fp);                                                                                        \
                                                                                                           \
        fprintf(stderr, "[" C_WARNING "WARNING" C_RESET "](" C_DOMAIN "" #D "" C_RESET "):" C_WARNING ""); \
                                                                                                           \
        va_start(v, format);                                                                               \
        vprintf(format, v);                                                                                \
        va_end(v);                                                                                         \
        fprintf(stderr, "" C_RESET "");                                                                    \
    }
#else //NOT LOG_TO_FILE
#define _DEFINE_WARNING_FUNCS(D)                                                                                   \
    static void warn##D(const char *format, ...);                                                                  \
    static inline void warn##D(const char *format, ...)                                                            \
    {                                                                                                              \
        fprintf(stderr, "[" C_WARNING "WARNING" C_LOG_RESET "](" C_DOMAIN "" #D "" C_LOG_RESET "):" C_WARNING ""); \
        va_list v;                                                                                                 \
        va_start(v, format);                                                                                       \
        vprintf(format, v);                                                                                        \
        va_end(v);                                                                                                 \
        fprintf(stderr, "" C_LOG_RESET "");                                                                        \
    }

#endif //END OF LOG_TO_FILE
#pragma endregion

#pragma region ERRORS
#ifdef LOG_TO_FILE
#define _DEFINE_ERROR_FUNCS(D)                                                                       \
    static void err##D(const char *format, ...);                                                     \
    static inline void err##D(const char *format, ...)                                               \
    {                                                                                                \
                                                                                                     \
        FILE *fp = fopen(log_name, "a");                                                             \
        va_list v;                                                                                   \
        va_start(v, format);                                                                         \
        fprintf(fp, "[ERROR](" #D "):");                                                             \
        vfprintf(fp, format, v);                                                                     \
        va_end(v);                                                                                   \
        fclose(fp);                                                                                  \
                                                                                                     \
        fprintf(stderr, "[" C_ERROR "ERROR" C_RESET "](" C_DOMAIN "" #D "" C_RESET "):" C_ERROR ""); \
                                                                                                     \
        va_start(v, format);                                                                         \
        vprintf(format, v);                                                                          \
        va_end(v);                                                                                   \
        fprintf(stderr, "" C_RESET "");                                                              \
        exit(1);                                                                                     \
    }
#else //LOG_TO_FILE
#define _DEFINE_ERROR_FUNCS(D)                                                                               \
    static void err##D(const char *format, ...);                                                             \
    static inline void err##D(const char *format, ...)                                                       \
    {                                                                                                        \
        fprintf(stderr, "[" C_ERROR "ERROR" C_LOG_RESET "](" C_DOMAIN "" #D "" C_LOG_RESET "):" C_ERROR ""); \
        va_list v;                                                                                           \
        va_start(v, format);                                                                                 \
        vprintf(format, v);                                                                                  \
        va_end(v);                                                                                           \
        fprintf(stderr, "" C_LOG_RESET "");                                                                  \
        exit(1);                                                                                             \
    }
#endif
#pragma endregion

#pragma region MESSAGES
#define _DEFINE_MESSAGE_FUNCS(D)                                                                   \
    static void msg##D(const char *format, ...);                                                   \
    static void logmsg##D(const char *format, ...);                                                \
    static inline void msg##D(const char *format, ...)                                             \
    {                                                                                              \
        va_list v;                                                                                 \
        fprintf(stdout, "[" C_MSG "MESSAGE" C_RESET "](" C_DOMAIN "" #D "" C_RESET "):" C_MSG ""); \
        va_start(v, format);                                                                       \
        vprintf(format, v);                                                                        \
        va_end(v);                                                                                 \
        fprintf(stdout, C_RESET);                                                                  \
    }                                                                                              \
    static inline void logmsg##D(const char *format, ...)                                          \
    {                                                                                              \
        va_list v;                                                                                 \
        fprintf(stdout, "[" C_MSG "MESSAGE" C_RESET "](" C_DOMAIN "" #D "" C_RESET "):" C_MSG ""); \
        fprintf(stdout, C_RESET);                                                                  \
        va_start(v, format);                                                                       \
        vlog##D(format, v);                                                                        \
        va_end(v);                                                                                 \
        va_start(v, format);                                                                       \
        vprintf(format, v);                                                                        \
        va_end(v);                                                                                 \
    }
#pragma endregion

#define _LOG_LOG(D) log##D
#define _LOG(D) _LOG_LOG(D)
#define log _LOG(DOMAIN)
#define _WARN_WARN(D) warn##D
#define _WARN(D) _WARN_WARN(D)
#define warn _WARN(DOMAIN)
#define _ERR_ERR(D) err##D
#define _ERR(D) _ERR_ERR(D)
#define err _ERR(DOMAIN)

#define _MSG_MSG(D) msg##D
#define _MSG(D) _MSG_MSG(D)
#define msg _MSG(DOMAIN)

#define _LMSG_MSG(D) logmsg##D
#define _LMSG(D) _LMSG_MSG(D)
#define logmsg _LMSG(DOMAIN)

DEFINE_DOMAIN(DOMAIN)
#endif