/**
  ******************************************************************************
  * @file    uart.h
  * @author
  * @version V1.1.0
  * @date    23-May-2019
  * @brief   This file contains the common defines and functions prototypes for
  *          the uart.c driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART3_H
#define __UART3_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "serial_common.h"
#include "app_uart_fifo.h"

/**
  * @brief  UART status structure definition
  */

/** @defgroup UART_Exported_Macro UART Exported Macro
  * @{
  */

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
#define UART_INSTANCE3_PRINTF(f_, ...)  do {\
                                          uint8_t cur_instance = putchar_instance;\
                                          putchar_instance = UART_INSTANCE3;      \
                                            fprintf(stdout, (f_), ##__VA_ARGS__);   \
                                            putchar_instance = cur_instance;        \
                                        } while(0)

#define UART3_PRINTF(f_, ...)           UART_INSTANCE3_PRINTF((f_), ##__VA_ARGS__)

#define LOG3_PRINTF(f_, ...)    				do {\
                                          UART_INSTANCE3_PRINTF("\r\nLOG3 ");\
                                          UART_INSTANCE3_PRINTF((f_), ##__VA_ARGS__);\
                                        } while(0)
#else
#define UART_INSTANCE3_PRINTF(f_, ...)
#define UART3_PRINTF(f_, ...)
#define LOG3_PRINTF(f_, ...)
#endif

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t uart_instance3_Init(app_uart_fifo_ctx_t *p_uart_cxt);

UART_Status_t uart_instance3_deinit(void);

int putchar_instance3(uint8_t c);

size_t uart_instance3_can_send(void);

void puts_uart_instance3(char *s);

size_t write_uart_instance3(uint8_t *s, size_t len);

int getchar_instance3(uint8_t *c);

size_t read_uart_instance3(uint8_t *s, size_t len);

size_t uart_instance3_available(void);

void uart_instance3_flush(void);

void uart_instance3_echo(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
