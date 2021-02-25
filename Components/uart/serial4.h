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
#ifndef __UART4_H
#define __UART4_H

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

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
#define UART_INSTANCE4_PRINTF(f_, ...)  do {\
                                            uint8_t cur_instance = putchar_instance;\
                                            putchar_instance = UART_INSTANCE4;      \
                                            fprintf(stdout, (f_), ##__VA_ARGS__);   \
                                            putchar_instance = cur_instance;        \
                                        } while(0)

#define UART4_PRINTF(f_, ...)           UART_INSTANCE4_PRINTF((f_), ##__VA_ARGS__)

#define LOG4_PRINTF(f_, ...)    				do {\
                                          UART_INSTANCE4_PRINTF("\r\nLOG4 ");\
                                          UART_INSTANCE4_PRINTF((f_), ##__VA_ARGS__);\
                                        } while(0)
#else
#define UART_INSTANCE4_PRINTF(f_, ...)
#define UART4_PRINTF(f_, ...)
#define LOG4_PRINTF(f_, ...)
#endif

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t uart_instance4_Init(app_uart_fifo_ctx_t *p_uart_cxt);

UART_Status_t uart_instance4_deinit(void);

int putchar_instance4(uint8_t c);

size_t uart_instance4_can_send(void);

void puts_uart_instance4(char *s);

size_t write_uart_instance4(uint8_t *s, size_t len);

int getchar_instance4(uint8_t *c);

size_t read_uart_instance4(uint8_t *s, size_t len);

void uart_instance4_rx_empty(void);

size_t uart_instance4_available(void);

void uart_instance4_flush(void);

void uart_instance4_echo(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
