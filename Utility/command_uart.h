#ifndef __COMMAND_UART_H
#define __COMMAND_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "uart.h"

typedef enum
{
	COMMAND_SUCCESS,
	COMMAND_LENGHT_ERROR,
	COMMAND_OF_FRAME_ERROR,
	COMMAND_DATA_OF_FRAME_ERROR,
	COMMAND_CRC_OF_FRAME_ERROR
} command_error_typdef;

typedef void (*update_str_panel_ptr)(char*, command_error_typdef);
void command_register_callback(update_str_panel_ptr p_callback);
void get_command_uart(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_UART_H */
