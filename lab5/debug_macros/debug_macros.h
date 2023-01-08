#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_PRINT_LOG
#define CONFIG_PRINT_LOG 1
#endif

// #ifndef CONFIG_FILE_LOG
// #define CONFIG_FILE_LOG 1
// #endif

// #ifndef CONFIG_DEBUG
// #define CONFIG_DEBUG 1
// #endif

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <macro.h>

#include <debug.h>

#ifdef __cplusplus
}
#endif

#endif
