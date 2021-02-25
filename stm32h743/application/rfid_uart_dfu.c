/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ticker.h"
#include "ymodem.h"
#include "serial4.h"
#include "Tools.h"
#include "delay.h"
#include "rfid_uart_dfu.h"
#include "crc32.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RFID_HEADER_LOCATE_ADDR (0x8020000)
#define RFID_APP_LOCATE_ADDR    (RFID_HEADER_LOCATE_ADDR + 16)
#define RFID_APP_SIZE_MAX (0x20000 - 0x8000 - 0x400)
#define DFU_WAIT_UPGRADE_CMD_TIMEOUT 10000 /* ms */
#define CRC_HEADER_LENGTH       12 /* number byte of fw_header calculator CRC32 */

#define BOOT_CMD_LENGTH 3
#define BOOT_CMD_SYNC   19
#define BOOT_CMD_ACCESS 25

/* Private macro -------------------------------------------------------------*/
#if (defined RFID_UART_DFU_DEBUG_ENABLE) && (RFID_UART_DFU_DEBUG_ENABLE == 1)
#include "serial_console_dbg.h"
#define UART_DFU_PRINTF(f_, ...) DBG_PRINTF(f_, ##__VA_ARGS__)
#define UART_DFU_TAG_PRINTF(f_, ...)      \
  do                                      \
  {                                       \
    DBG_PRINTF("\r\n[U_DFU] ");      \
    DBG_PRINTF((f_), ##__VA_ARGS__); \
  } while (0)

#else
#define UART_DFU_PRINTF(f_, ...)
#define UART_DFU_TAG_PRINTF(f_, ...)
#endif

/* Private variables ---------------------------------------------------------*/
const char filename_upload[] = "rfid_reader.bin";

const char rs232_exit_cmd[BOOT_CMD_LENGTH + 1] = "CCC";
const char rs232_boot_cmd[BOOT_CMD_LENGTH + 1] = "BBB";
const char rs232_boot_sync[BOOT_CMD_SYNC + 1] = "\r\nRFID_BOOTLOADER\r\n";
const char rs232_boot_access[BOOT_CMD_ACCESS + 1] =
		"\r\nRFID_ENTER_BOOTLOADER\r\n";
const uint8_t force_reset_str[] = { 0x7E, 0x02, 0x13, 0x11, 0x7F };
const uint8_t abort_cmd[] = { ABORT1, ABORT2, ABORT1, ABORT2 };

static ymodem_cxt_t ymodem_handle;
static uint8_t dfu_process = 0;

/* Private function prototypes -----------------------------------------------*/
static uint8_t ymodem_rx_blocking(uint8_t *pData, uint16_t Size,
		uint32_t Timeout);
static uint8_t ymodem_tx_blocking(uint8_t *pData, uint16_t Size,
		uint32_t Timeout);
static uint8_t dfu_data_read(uint32_t addr, uint32_t *src,
		uint32_t lengthInBytes);
static uint8_t SerialUpload(ymodem_cxt_t *ymodem);
static uint8_t app_verify(uint32_t size);

uint8_t rfid_dfu_is_update(void) {
	return dfu_process;
}

static uint8_t dfu_data_read(uint32_t addr, uint32_t *src,
		uint32_t lengthInBytes) {

	/* Read data from internal memory */
	 memcpy((uint8_t*)src, (uint8_t*)addr, lengthInBytes);

	/* Read data from external memory */
//	ExtFlash_read(addr, lengthInBytes, (uint8_t*) src);

	return 1;
}

/* Brief: The function process DFU RFID module
 * Return: 0(RFID_DFU_OK), 1(RFID_DFU_ERROR)
 *
 * Note: This function is blocking process to handle ymodem portocol update rfid firmware
 * */
int uart_dfu_process(void) {
  ticker_t dfu_timeout;
	rfid_dfu_FirmwareHeader_t fw_header;
	uint32_t header_size;
	uint8_t sync_id = 0;
	uint8_t exit_id = 0;
	uint8_t access_id = 0;
	uint8_t err_cnt = 0;
	uint8_t error_status = 0;
	uint8_t result = RFID_DFU_ERROR;

	dfu_process = 1;

  if(app_verify(RFID_APP_LOCATE_ADDR) == RFID_DFU_OK)
  {
    header_size = sizeof(rfid_dfu_FirmwareHeader_t);
    /* Get firmware header */
    dfu_data_read(RFID_HEADER_LOCATE_ADDR, (uint32_t*)&fw_header, header_size);

    /* Init ymodem object */
    ymodem_handle.input_cb = ymodem_rx_blocking;
    ymodem_handle.output_cb = ymodem_tx_blocking;

    /* Name of image */
    ymodem_handle.flash.name = filename_upload;

    /* Register callback function to read data of application image */
    ymodem_handle.flash_read_cb = dfu_data_read;

    /* Address begin of application image stored in flash
     * If using file system, it should be begin by Zero
     * If using external memory raw data, it should be address begin stored app image
     * */
    ymodem_handle.flash.start_addr = RFID_APP_LOCATE_ADDR + 0; /* + header_size */

    /* Max Size of application image */
    ymodem_handle.flash.max_size = RFID_APP_SIZE_MAX;

    /* Size of application image */
    ymodem_handle.flash.size = fw_header.firmwareSize;

    UART_DFU_TAG_PRINTF("Waiting sync boot command");
    ymodem_tx_blocking((uint8_t*) abort_cmd, sizeof(abort_cmd), TX_TIMEOUT);
    ymodem_tx_blocking((uint8_t*) force_reset_str, sizeof(force_reset_str),
      TX_TIMEOUT);
    delay(100);
    uart_instance4_rx_empty();
    ticker_begin(&dfu_timeout, DFU_WAIT_UPGRADE_CMD_TIMEOUT);
    while (!ticker_expried(&dfu_timeout)) {
      uint8_t c;
      uint8_t result;
      result = ymodem_rx_blocking(&c, 1, RX_TIMEOUT);
      if (HAL_USER_TIMEOUT == result) {
        if (++err_cnt >= MAX_ERRORS) {
          error_status = 1; /* Error */
          UART_DFU_TAG_PRINTF("TIMEOUT BOOT");
          break;
        } else {
          ymodem_tx_blocking((uint8_t*) abort_cmd, sizeof(abort_cmd),
                  TX_TIMEOUT);
          ymodem_tx_blocking((uint8_t*) force_reset_str,
              sizeof(force_reset_str), TX_TIMEOUT);
          delay(100);
          uart_instance4_rx_empty();
        }
      } else {
        UART_DFU_PRINTF("%c", c);
        if (check_strncmp((char*) rs232_boot_sync, c, &sync_id,
        BOOT_CMD_SYNC)) {
          ymodem_tx_blocking((uint8_t*) rs232_boot_cmd, BOOT_CMD_LENGTH,
          TX_TIMEOUT);
          UART_DFU_TAG_PRINTF("rx sync boot cmd");
        }

        if (check_strncmp((char*)rs232_exit_cmd, c, &exit_id, BOOT_CMD_LENGTH))
          {
          ymodem_tx_blocking((uint8_t*) abort_cmd, sizeof(abort_cmd), TX_TIMEOUT);
          delay(100);
          uart_instance4_rx_empty();
          UART_DFU_TAG_PRINTF("Restart Ymodem");
          }

        if (check_strncmp((char*) rs232_boot_access, c, &access_id,
        BOOT_CMD_ACCESS)) {
          UART_DFU_TAG_PRINTF("Boot access");
          break;
        }
      }
    }

    if (!error_status) {
      result = SerialUpload(&ymodem_handle);
    }

    if (result != RFID_DFU_OK) {
      UART_DFU_TAG_PRINTF("Error Occurred while Transmitting File");
    } else {
      UART_DFU_TAG_PRINTF("File uploaded successfully");
    }
  }
  else
  {
    UART_DFU_TAG_PRINTF("App crc verify failure");
  }
	dfu_process = 0;

	UART_DFU_PRINTF("\r\n");

	return result;
}

/* Return 0(OK), 1(Error)
 * */
static uint8_t SerialUpload(ymodem_cxt_t *ymodem) {
	uint8_t err_cnt = 0;
	uint8_t status = RFID_DFU_OK;

	UART_DFU_TAG_PRINTF("File Size: %lu\n", ymodem->flash.size);

	UART_DFU_TAG_PRINTF("Select Transmit File");
	uart_instance4_rx_empty();
	while (1) {
	  uint8_t c;
		uint8_t result;
		result = ymodem_rx_blocking(&c, 1, RX_TIMEOUT);
		if (HAL_USER_TIMEOUT == result) {
			status = RFID_DFU_ERROR; /* Error */
			break;
		}
		if (c == CRC16) {
			/* Transmit the flash image through ymodem protocol */
			status = Ymodem_Transmit(ymodem, ymodem->flash.start_addr,
					ymodem->flash.name, ymodem->flash.size);
		} else {
			if (++err_cnt >= MAX_ERRORS) {
				status = RFID_DFU_ERROR; /* Error */
			}
		}
		break;
	}

	return status;
}

/*
Return:
0: OK
1: Error
*/
static uint8_t app_verify(uint32_t addr)
{
#define FW_BUFF_LENGTH 256
  rfid_dfu_FirmwareHeader_t fw_header;
  uint32_t header_size;
  uint32_t fw_size;
  uint32_t crc;
  uint32_t buff_length;
  uint8_t  fw_buff[FW_BUFF_LENGTH]; /* Multiples 4byte */
  uint8_t result = RFID_DFU_ERROR;

  UART_DFU_TAG_PRINTF("Verify app file");
  header_size = sizeof(rfid_dfu_FirmwareHeader_t);
  dfu_data_read(RFID_HEADER_LOCATE_ADDR, (uint32_t*)&fw_header, header_size);

  if(RFID_APP_SIZE_MAX < fw_header.firmwareSize)
  {
    UART_DFU_TAG_PRINTF("File header size Error");
    UART_DFU_TAG_PRINTF("File size     : %lu", fw_header.firmwareSize);
    UART_DFU_TAG_PRINTF("RFID size max : %u", RFID_APP_SIZE_MAX);
  }
  else
  {
    if(RFID_FW_TYPE == fw_header.firmwareType)
    {
      UART_DFU_TAG_PRINTF("Fw_Type header OK");
      UART_DFU_TAG_PRINTF("Calculator CRC start");

      /* Update addr application image start */
      addr += 0; /* + header_size */
      fw_size = fw_header.firmwareSize;

      /* Calculator CRC */
      CRC32_Start(0);
      CRC32_Accumulate((uint8_t *) &(fw_header.firmwareSize), CRC_HEADER_LENGTH);

      while(fw_size)
      {
        buff_length = FW_BUFF_LENGTH;
        if(fw_size < FW_BUFF_LENGTH)
        {
          buff_length = fw_size;
        }

        dfu_data_read(addr, (uint32_t*)fw_buff, buff_length);
        CRC32_Accumulate(fw_buff, buff_length);

        addr += buff_length;
        fw_size -= buff_length;
      }

      crc = CRC32_Get();
      if(crc == fw_header.checksumValue)
      {
        result = RFID_DFU_OK;
        UART_DFU_TAG_PRINTF("\r\nApp_crc verify OK");
      }
      else
      {
        UART_DFU_TAG_PRINTF("\r\nApp_crc verify Error");
        UART_DFU_TAG_PRINTF("File crc        : 0x%08lX", crc);
        UART_DFU_TAG_PRINTF("RFID Header crc : 0x%08lX", fw_header.checksumValue);
      }
    }
    else
    {
      UART_DFU_TAG_PRINTF("File header type Error");
      UART_DFU_TAG_PRINTF("File type        : 0x%08lX", fw_header.firmwareType);
      UART_DFU_TAG_PRINTF("RFID Header type : 0x%08lX", RFID_FW_TYPE);
    }
  }

  return result;
}


/*
 typedef enum
 {
 HAL_USER_OK       = 0x00,
 HAL_USER_ERROR    = 0x01,
 HAL_USER_BUSY     = 0x02,
 HAL_USER_TIMEOUT  = 0x03
 } HAL_USER_StatusTypeDef;
 */
static uint8_t ymodem_rx_blocking(uint8_t *pData, uint16_t Size,
		uint32_t Timeout) {
	ticker_t u_timeout;
	uint8_t result = HAL_USER_TIMEOUT;

	ticker_begin(&u_timeout, Timeout);
	while (!ticker_expried(&u_timeout)) {
		if (UART_OK == getchar_instance4(pData)) {
			++pData;
			--Size;
			if (0 == Size) {
				result = HAL_USER_OK;
				break;
			}
		}
	}

	return result;
}

static uint8_t ymodem_tx_blocking(uint8_t *pData, uint16_t Size,
		uint32_t Timeout) {
	ticker_t u_timeout;
	uint32_t tx_size;
	uint8_t result;

//	uart_instance4_rx_empty();
//	while(!uart_instance4_tx_is_empty()) {};

	result = HAL_USER_TIMEOUT;
	ticker_begin(&u_timeout, Timeout);
	while (!ticker_expried(&u_timeout)) {
		tx_size = uart_instance4_can_send();
		if (tx_size > Size) {
			tx_size = Size;
		}

		if (tx_size) {
			write_uart_instance4(pData, tx_size);
			pData += tx_size;
			Size -= tx_size;
			if (0 == Size) {
				result = HAL_USER_OK;
				break;
			}
		}
	}

	return result;
}
