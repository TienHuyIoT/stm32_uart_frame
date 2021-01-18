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
#ifndef __UART1_H
#define __UART1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "serial_common.h"
#include "app_uart_fifo.h"

/** @defgroup UART_Exported_Macro UART Exported Macro
  * @{
  */
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
#define UART_INSTANCE1_PRINTF(f_, ...)  do {\
                                          uint8_t cur_instance = putchar_instance;\
                                          putchar_instance = UART_INSTANCE1;      \
                                            fprintf(stdout, (f_), ##__VA_ARGS__);   \
                                            putchar_instance = cur_instance;        \
                                        } while(0)

#define UART1_PRINTF(f_, ...)           UART_INSTANCE1_PRINTF((f_), ##__VA_ARGS__)

#define LOG1_PRINTF(f_, ...)    				do {\
                                          UART_INSTANCE1_PRINTF("\r\nLOG1 ");\
                                          UART_INSTANCE1_PRINTF((f_), ##__VA_ARGS__);\
                                        } while(0)
#else
#define UART_INSTANCE1_PRINTF(f_, ...)
#define UART1_PRINTF(f_, ...)
#define LOG1_PRINTF(f_, ...)
#endif

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t uart_instance1_Init(app_uart_fifo_ctx_t *p_uart_cxt);

UART_Status_t uart_instance1_deinit(void);

int putchar_instance1(uint8_t c);

uint32_t uart_instance1_can_send(void);

void puts_uart_instance1(char *s);

uint32_t write_uart_instance1(uint8_t *s, uint32_t len);

int getchar_instance1(uint8_t *c);

uint32_t read_uart_instance1(uint8_t *s, uint32_t len);

uint32_t uart_instance1_available(void);

void uart_instance1_flush(void);

void uart_instance1_echo(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
