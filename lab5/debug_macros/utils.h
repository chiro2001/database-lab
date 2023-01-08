#ifndef __UTILS_H__
#define __UTILS_H__

#include <debug_macros.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANSI_FG_BLACK "\33[1;30m"
#define ANSI_FG_RED "\33[1;31m"
#define ANSI_FG_GREEN "\33[1;32m"
#define ANSI_FG_YELLOW "\33[1;33m"
#define ANSI_FG_BLUE "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN "\33[1;36m"
#define ANSI_FG_WHITE "\33[1;37m"
#define ANSI_BG_BLACK "\33[1;40m"
#define ANSI_BG_RED "\33[1;41m"
#define ANSI_BG_GREEN "\33[1;42m"
#define ANSI_BG_YELLOW "\33[1;43m"
#define ANSI_BG_BLUE "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN "\33[1;46m"
#define ANSI_BG_WHITE "\33[1;47m"
#define ANSI_NONE "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

#ifdef CONFIG_FILE_LOG

extern FILE *log_fp;

#ifndef CONFIG_TAIL_LOG
#define log_write(...)              \
  do {                              \
    extern FILE *log_fp;            \
    if (log_fp != NULL) {           \
      fprintf(log_fp, __VA_ARGS__); \
      fflush(log_fp);               \
    }                               \
  } while (0)
#else
#define log_write(...)                          \
  do {                                          \
    extern FILE *log_fp;                        \
    extern char *log_path;                      \
    extern char *log_tail_path;                 \
    extern uint64_t log_cnt;                    \
    extern void init_log(const char *log_file); \
    if (log_fp != NULL) {                       \
      fprintf(log_fp, __VA_ARGS__);             \
      fflush(log_fp);                           \
      log_cnt++;                                \
      if (log_cnt == CONFIG_TAIL_LOG_SIZE) {    \
        fclose(log_fp);                         \
        rename(log_path, log_tail_path);        \
        init_log(log_path);                     \
        log_cnt = 0;                            \
      }                                         \
    }                                           \
  } while (0)
#endif

#else

#define log_write(...)

#endif

#ifdef CONFIG_PRINT_LOG
#define _Log(console, ...)               \
  do {                                   \
    IFDEF(console, printf(__VA_ARGS__)); \
    IFDEF(console, fflush(stdout));      \
    log_write(__VA_ARGS__);              \
  } while (0)

#define _Dbg(console, ...)               \
  do {                                   \
    IFDEF(console, IFDEF(CONFIG_DEBUG, printf(__VA_ARGS__))); \
    IFDEF(console, IFDEF(CONFIG_DEBUG, fflush(stdout)));      \
    IFDEF(CONFIG_DEBUG, log_write(__VA_ARGS__));              \
  } while (0)

#define _Err(console, ...)                        \
  do {                                            \
    IFDEF(console, fprintf(stderr, __VA_ARGS__)); \
    IFDEF(console, fflush(stderr));               \
    log_write(__VA_ARGS__);                       \
  } while (0)

#else
#define _Log(...)           \
  do {                      \
    log_write(__VA_ARGS__); \
  } while (0)

#define _Err(...)           \
  do {                      \
    log_write(__VA_ARGS__); \
  } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
