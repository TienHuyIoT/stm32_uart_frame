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
#ifndef __UART7_H
#define __UART7_H

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
#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
#define UART_INSTANCE7_PRINTF(f_, ...)  do {\
                                          uint8_t cur_instance = putchar_instance;\
                                          putchar_instance = UART_INSTANCE7;      \
                                          fprintf(stdout, (f_), ##__VA_ARGS__);   \
                                          putchar_instance = cur_instance;        \
                                        } while(0)

#define UART7_PRINTF(f_, ...)           UART_INSTANCE7_PRINTF((f_), ##__VA_ARGS__)

#define LOG7_PRINTF(f_, ...)            do {\
                                          UART_INSTANCE7_PRINTF("\r\nLOG7 ");\
                                          UART_INSTANCE7_PRINTF((f_), ##__VA_ARGS__);\
                                        } while(0)
#else
#define UART_INSTANCE7_PRINTF(f_, ...)
#define UART7_PRINTF(f_, ...)
#define LOG7_PRINTF(f_, ...)
#endif

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t uart_instance7_Init(app_uart_fifo_ctx_t *p_uart_cxt);

UART_Status_t uart_instance7_deinit(void);

int putchar_instance7(uint8_t c);

size_t uart_instance7_can_send(void);

void puts_uart_instance7(char *s);

size_t write_uart_instance7(uint8_t *s, size_t len);

int getchar_instance7(uint8_t *c);

size_t read_uart_instance7(uint8_t *s, size_t len);

size_t uart_instance7_available(void);

void uart_instance7_flush(void);

void uart_instance7_echo(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
