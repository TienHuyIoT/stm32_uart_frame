/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_DFU_H
#define __UART_DFU_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define RFID_UART_DFU_DEBUG_ENABLE 1
#define RFID_FW_TYPE          ((uint32_t)0x00000701)

typedef enum {
  RFID_DFU_OK = 0,
  RFID_DFU_ERROR
} rfid_dfu_status_t;

/** @brief Firmware info */
typedef struct __attribute__((packed, aligned(4)))
{
    /**< Checksum value */
    uint32_t checksumValue;
    /**< Firmware size (inbytes)  */
    uint32_t firmwareSize;
    /**< Firmware type (ECU, SCU, HMI ...)  */
    uint32_t firmwareType;
    /**< Firmware version: <Major(1 byte).Minor(1 byte).Subminor(2 bytes)>  */
    uint32_t firmwareVersion;
} rfid_dfu_FirmwareHeader_t;

uint8_t rfid_dfu_is_update(void);

/* Brief: The function process DFU RFID module
 * Return: 0(OK), 1(fail)
 *
 * Note: This function is blocking process to handle ymodem portocol update rfid firmware
 * */
int uart_dfu_process(void);

#endif  /* __UART_DFU_H */
