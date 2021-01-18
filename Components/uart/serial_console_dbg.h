#ifndef _SERIAL_CONSOLE_DBG_H
#define _SERIAL_CONSOLE_DBG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "serial3.h"

#define DBG_PRINTF(f_, ...)           UART3_PRINTF((f_), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
