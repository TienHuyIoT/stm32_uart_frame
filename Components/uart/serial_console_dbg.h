#ifndef _SERIAL_CONSOLE_CONSOLE_H
#define _SERIAL_CONSOLE_CONSOLE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "serial3.h"

#define DBG_PRINTF(f_, ...)           UART3_PRINTF((f_), ##__VA_ARGS__)

#define g_debugLevel 0

#define CONSOLE_LOGE(x, ...) {if(g_debugLevel >= 0) {DBG_PRINTF("E %s: ",x); DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n");}}
#define CONSOLE_LOGW(x, ...) {if(g_debugLevel >= 1) {DBG_PRINTF("W %s: ",x); DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n");}}
#define CONSOLE_LOGI(x, ...) {if(g_debugLevel >= 2) {DBG_PRINTF("I %s: ",x); DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n");}}
#define CONSOLE_LOGD(x, ...) {if(g_debugLevel >= 3) {DBG_PRINTF("D %s: ",x); DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n");}}
#define CONSOLE_LOGV(x, ...) {if(g_debugLevel >= 4) {DBG_PRINTF("V %s: ",x); DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n");}}

#ifdef __cplusplus
}
#endif

#endif
