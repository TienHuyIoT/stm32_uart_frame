/**
 ******************************************************************************
 * @file    common.h
 * @author
 * @version V1.1.0
 * @date    23-May-2019
 * @brief   This file contains the common defines and functions prototypes for
 *          the common.c driver.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SERIAL_COMMON_H
#define _SERIAL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* If using define value = 1, not use define value = 0 */

#define APP_UART_FIFO_1	   0
#define APP_UART_FIFO_2	   0
#define APP_UART_FIFO_3	   1
#define APP_UART_FIFO_4	   0
#define APP_UART_FIFO_5	   0
#define APP_UART_FIFO_6	   0
#define APP_UART_FIFO_7	   0
#define APP_UART_FIFO_8	   0

/**
 * @brief  UART status structure definition
 */
typedef enum
{
	UART_OK = 0,
	UART_ERROR
} UART_Status_t;

typedef enum
{
	UART_INSTANCE1,
	UART_INSTANCE2,
	UART_INSTANCE3,
	UART_INSTANCE4,
	UART_INSTANCE5,
	UART_INSTANCE6,
	UART_INSTANCE7,
	UART_INSTANCE8,
	UART_INSTANCE_NUM
} putchar_instance_t;

extern uint8_t putchar_instance;

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
