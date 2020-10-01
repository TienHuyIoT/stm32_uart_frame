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
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>

 #include "app_uart_fifo.h"

/**
  * @brief  UART status structure definition
  */

typedef enum {
	UART_OK = 0,
	UART_ERROR
} UART_Status_t;

/** @defgroup UART_Exported_Macro UART Exported Macro
  * @{
  */

#define UART_INSTANCE0_PRINTF(f_, ...)          fprintf(stdout, (f_), ##__VA_ARGS__)

#define _PRINTF(f_, ...)                    	UART_INSTANCE0_PRINTF((f_), ##__VA_ARGS__)

#define LOG_PRINTF(f_, ...)    				do {\
											  UART_INSTANCE0_PRINTF("\r\nLOG ");\
                                      	  	  UART_INSTANCE0_PRINTF((f_), ##__VA_ARGS__);\
                                    		} while(0)

/** @defgroup _UART_Exported_Functions UART Exported Functions
  * @{
  */
UART_Status_t uart_instance0_Init(app_uart_fifo_ctx_t *p_uart_cxt);

UART_Status_t uart_instance0_deinit(void);

UART_Status_t putchar_instance0(char c);

uint32_t uart_instance0_can_send(void);

void puts_uart_instance0(char *s);

uint32_t write_uart_instance0(uint8_t *s, uint32_t len);

UART_Status_t getchar_instance0(char *c);

uint32_t uart_instance0_available(void);

void uart_instance0_flush(void);

void uart_test_echo(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
