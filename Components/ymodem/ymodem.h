/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __YMODEM_H_
#define __YMODEM_H_

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  Comm status structures definition
  */
typedef enum
{
  COM_OK       = 0x00,
  COM_ERROR    = 0x01,
  COM_ABORT    = 0x02,
  COM_TIMEOUT  = 0x03,
  COM_DATA     = 0x04,
  COM_LIMIT    = 0x05
} COM_StatusTypeDef;

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_USER_OK       = 0x00,
  HAL_USER_ERROR    = 0x01,
  HAL_USER_BUSY     = 0x02,
  HAL_USER_TIMEOUT  = 0x03
} HAL_USER_StatusTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
#define YMODEM_DEBUG_ENABLE     1

/* Packet structure defines */
#define PACKET_HEADER_SIZE      ((uint32_t)3)
#define PACKET_DATA_INDEX       ((uint32_t)4)
#define PACKET_START_INDEX      ((uint32_t)1)
#define PACKET_NUMBER_INDEX     ((uint32_t)2)
#define PACKET_CNUMBER_INDEX    ((uint32_t)3)
#define PACKET_TRAILER_SIZE     ((uint32_t)2)
#define PACKET_OVERHEAD_SIZE    (PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE - 1)
#define PACKET_SIZE             ((uint32_t)128)
#define PACKET_1K_SIZE          ((uint32_t)1024)

/* /-------- Packet in IAP memory ------------------------------------------\
 * | 0      |  1    |  2     |  3   |  4      | ... | n+4     | n+5  | n+6  | 
 * |------------------------------------------------------------------------|
 * | unused | start | number | !num | data[0] | ... | data[n] | crc0 | crc1 |
 * \------------------------------------------------------------------------/
 * the first byte is left unused for memory alignment reasons                 */

#define FILE_NAME_LENGTH        ((uint32_t)64)
#define FILE_SIZE_LENGTH        ((uint32_t)16)

#define SOH                     ((uint8_t)0x01)  /* start of 128-byte data packet */
#define STX                     ((uint8_t)0x02)  /* start of 1024-byte data packet */
#define EOT                     ((uint8_t)0x04)  /* end of transmission */
#define ACK                     ((uint8_t)0x06)  /* acknowledge */
#define NAK                     ((uint8_t)0x15)  /* negative acknowledge */
#define CA                      ((uint32_t)0x18) /* two of these in succession aborts transfer */
#define CRC16                   ((uint8_t)0x43)  /* 'C' == 0x43, request 16-bit CRC */
#define NEGATIVE_BYTE           ((uint8_t)0xFF)

#define ABORT1                  ((uint8_t)0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  ((uint8_t)0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             ((uint32_t)3000)
#define DOWNLOAD_TIMEOUT        ((uint32_t)1000) /* 10 second retry delay */
#define MAX_ERRORS              ((uint32_t)5)

#define TX_TIMEOUT              ((uint32_t)100)
#define RX_TIMEOUT              ((uint32_t)3000)

typedef uint8_t  (*ymodem_rx_cb)(uint8_t*, uint16_t, uint32_t);
typedef uint8_t  (*ymodem_tx_cb)(uint8_t*, uint16_t, uint32_t);
typedef uint8_t (*mem_write_cb)(uint32_t addr_start, uint32_t *src, uint32_t lengthInBytes);
typedef uint8_t (*mem_read_cb)(uint32_t addr_start, uint32_t *src, uint32_t lengthInBytes);
typedef uint8_t (*mem_erase_cb)(uint32_t addr_start, uint32_t lengthInBytes);


typedef struct
{
  uint32_t start_addr;
  uint32_t size;
  uint32_t max_size;
  const char* name;
} ymem_info_t;

typedef struct
{
  ymodem_rx_cb input_cb;
  ymodem_tx_cb output_cb;
  mem_write_cb flash_write_cb;
  mem_read_cb  flash_read_cb;
  mem_erase_cb flash_erase_cb;
  ymem_info_t  flash;
} ymodem_cxt_t;

/* Exported functions ------------------------------------------------------- */
COM_StatusTypeDef Ymodem_Receive(ymodem_cxt_t* ymodem, uint32_t *p_size);
COM_StatusTypeDef Ymodem_Transmit(ymodem_cxt_t* ymodem, uint32_t index, const char *p_file_name, uint32_t file_size);

#ifdef __cplusplus
}
#endif

#endif  /* __YMODEM_H_ */
