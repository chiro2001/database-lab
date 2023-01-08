#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <debug_macros.h>
#include <stdio.h>
#include <utils.h>
#include <time.h>

#define ENVR
// #define ENVR ANSI_FMT("REF", ANSI_FG_RED ANSI_BG_YELLOW) " "

#ifndef CONFIG_LOG_COLOR1
#define CONFIG_LOG_COLOR1 ANSI_FG_BLUE
#endif

#ifndef CONFIG_LOG_COLOR2
#define CONFIG_LOG_COLOR2 ANSI_FG_RED
#endif

#define Log(format, ...) \
    _Log(CONFIG_PRINT_LOG, ENVR MUXDEF(CONFIG_LOG_COLOR_DISABLED, \
        "[%s:%d %s] " format, ANSI_FMT("[%s:%d %s] " format, CONFIG_LOG_COLOR1)) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Err(format, ...) \
    _Err(CONFIG_PRINT_LOG, ENVR MUXDEF(CONFIG_LOG_COLOR, \
        "[%s:%d %s] " format, ANSI_FMT("[%s:%d %s] " format, CONFIG_LOG_COLOR2)) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Dbg(format, ...) \
    _Dbg(CONFIG_PRINT_LOG, ENVR MUXDEF(CONFIG_LOG_COLOR_DISABLED, \
        "[%s:%d %s] " format, ANSI_FMT("[%s:%d %s] " format, CONFIG_LOG_COLOR1)) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      MUXDEF(CONFIG_TARGET_AM, printf(ANSI_FMT(format, CONFIG_LOG_COLOR2) "\n", ## __VA_ARGS__), \
        (fflush(stdout), fprintf(stderr, ANSI_FMT(format, CONFIG_LOG_COLOR2) "\n", ##  __VA_ARGS__))); \
      assert(cond); \
    } \
  } while (0)

#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#define TODO() panic("please implement me")

#ifdef __cplusplus
}
#endif

#endif
