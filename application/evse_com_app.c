#include "ticker.h"
#include "frame.h"
#include "frame_com.h"
#include "evse_com_app.h"

#if (defined EVSE_COM_DEBUG_ENABLE) && (EVSE_COM_DEBUG_ENABLE == 1)
#include "serial3.h"
#define EVSE_PRINTF(f_, ...)               _PRINTF(f_, ##__VA_ARGS__)
#define EVSE_TAG_PRINTF(f_, ...)           do {\
                                               _PRINTF("\r\nFRAME. ");\
                                               _PRINTF((f_), ##__VA_ARGS__);\
                                               } while(0)
#else
#define EVSE_PRINTF(f_, ...)
#define EVSE_TAG_PRINTF(f_, ...)
#endif

#define FRAME_EVSE_ACK	0
#define FRAME_EVSE_NACK	1

#define FRAME_INPUT_OK	UART_OK

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define COMMAND_RX_BUFF_SIZE FRAME_SIZE_MAX
#define COMMAND_TX_BUFF_SIZE FRAME_SIZE_MAX

/* Private variables ---------------------------------------------------------*/
/**@brief String literals for the serial command. */
#if (defined EVSE_COM_DEBUG_ENABLE) && (EVSE_COM_DEBUG_ENABLE == 1)
static char const * lit_serialid[] = {
/* 0 */ "FRAME_EVSE_HEART_BEAT",
/* 1 */ "FRAME_EVSE_UID",
/* 2 */ "FRAME_EVSE_KEYA",
/* 3 */ "FRAME_EVSE_SECTOR",
/* 4 */ "FRAME_EVSE_AES",
/* 5 */ "FRAME_EVSE_RTC"
};
#endif

/* Variables uart ttl interface */
static uint8_t uart_ttl_rx_buff[COMMAND_RX_BUFF_SIZE];
static uint8_t uart_ttl_tx_buff[COMMAND_TX_BUFF_SIZE];

static frame_com_cxt_t frame_com_uart_ttl_cxt;

static rfid_frame_handle_t *frame_handle;

/* Private function prototypes -----------------------------------------------*/
static void command_receive_event_handle(frame_com_cxt_t* frame_instance, uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length);
static void command_send_callback(frame_com_cxt_t* frame_instance, uint8_t* buff, uint16_t length);
static void command_parse_process(void);

void evse_handle_loop(void)
{
	/* Get command event process */
    command_parse_process();
}

void evse_handle_init(rfid_frame_handle_t *fp_callback)
{
	//uint8_t data_buf[4] = {'1' , '2', '3', '4'};

	frame_handle = fp_callback;

	/* Init object handle communicate frame */
	frame_com_begin(&frame_com_uart_ttl_cxt,
			command_receive_event_handle,
			command_send_callback,
			uart_ttl_tx_buff,
			uart_ttl_rx_buff,
			COMMAND_TX_BUFF_SIZE,
			COMMAND_RX_BUFF_SIZE);

	/* API send command (*frame_com_cxt, cmd, *data, length) */

	/* For test create the frame buffer and send to serial */
	//frame_com_send(&frame_com_uart_ttl_cxt, (uint8_t)FRAME_EVSE_UID, data_buf, sizeof(data_buf));

	/* For test parse the frame buffer. Using the "hercules terminal"
	 * paste the string $7E$06$011234$03$7F and send
	*/
}

static void command_parse_process(void)
{
	frame_size get_cnt;
	uint8_t data;

	get_cnt = FRAME_SIZE_MAX/2; /* counter limit once getchar from serial */
	if(frame_handle->input_cb)
	{
		while (FRAME_INPUT_OK == frame_handle->input_cb(&data))
		{
			// input data into utilities frame
			if(FRAME_COM_FINISH == frame_com_data_in(&frame_com_uart_ttl_cxt, data))
			{
				break;
			}

			// Avoid while loop forever if uart_rx so much
			--get_cnt;
			if(0 == get_cnt) {
				break;
			}
		}
	}
}

/* Brief: the function callback to send frame buff over serial
 *        once call the function frame_com_send()
 * */
static void command_send_callback(frame_com_cxt_t* frame_instance, uint8_t* buff, uint16_t length)
{
	EVSE_PRINTF("\r\nSend a frame buffer with length = %u", length);
	EVSE_PRINTF("\r\nBuffer: ");
	for(frame_size i = 0; i < length; ++i)
	{
		EVSE_PRINTF("%02X ",buff[i]);
	}
	EVSE_PRINTF("\r\n");


	if(frame_instance == &frame_com_uart_ttl_cxt)
	{
		if(frame_handle->output_cb)
		{
			for(uint16_t i = 0; i < length; ++i)
			{
				frame_handle->output_cb(buff[i]);
			}
		}
	}
}

/* Brief: the function event callback for the parse a frame message */
static void command_receive_event_handle(frame_com_cxt_t* frame_instance, uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length)
{
	if((uint8_t)FRAME_OK == result)
	{
		if(FRAME_EVSE_NUM <= cmd)
		{
			EVSE_PRINTF("\r\nCMD error\r\n");
			return;
		}

		EVSE_PRINTF("\r\nGet new frame");
		EVSE_PRINTF("\r\n- CMD: %s", lit_serialid[cmd]);
		EVSE_PRINTF("\r\n- Data length: %u", length);
		EVSE_PRINTF("\r\n- Data: ");
		for(uint16_t i = 0; i < length; ++i)
		{
			EVSE_PRINTF("%02X ", data[i]);
		}
		EVSE_PRINTF("\r\n");

		switch (cmd)
		{
		/* HEART BEAT command
		 * Hercules terminal test string: $7E$02$00$02$7F
		 * Hex recieve: {7E}{02}{00}{02}{7F}
		 * Hex ACK response: {7E}{03}{00}{00}{03}{7F}
		 * */
		case FRAME_EVSE_HEART_BEAT:
		{
			uint8_t df_buffer[1];
			// check data length of heart beat command
			if(0 == length)
			{
				df_buffer[0] = FRAME_EVSE_ACK;
			}
			else
			{
				df_buffer[0] = FRAME_EVSE_NACK;
			}
			// response ack/nack to evse
			frame_com_send(frame_instance, (uint8_t)FRAME_EVSE_HEART_BEAT, df_buffer, sizeof(df_buffer));
			break;
		}

		/* UID command
		 * UID: 0xD2 0xEF 0x46 0xD2
		 * Hex string 0x7E 0x12 0x01 0xD2 0xEF 0x46 0xD2 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xA9 0x13 0x7F
		 * Hercules terminal test string: $7E$12$01$D2$EF$46$D2$00$00$00$00$00$00$00$00$00$00$00$A9$13$7F
		 * */
		case FRAME_EVSE_UID:
		{
			uint8_t df_buffer[1];
			// check data length of uid command
			if(UID_DF_LENGTH == length)
			{
				uint8_t crc = 0;
				// verify checksum
				for(uint8_t i = 0; i < UID_DF_LENGTH; ++i)
				{
					crc ^= data[i];
				}

				/* Crc is true */
				if (0 == crc)
				{
					/* Call cb function */
					if(frame_handle->uid_cb)
					{
						frame_handle->uid_cb(&data[UID_DF_UID_INDEX], data[UID_DF_UID_LENGTH_INDEX]);
					}

					df_buffer[0] = FRAME_EVSE_ACK;
					EVSE_PRINTF("\r\nEVSE Response ACK");
				}
				else
				{
					df_buffer[0] = FRAME_EVSE_NACK;
					EVSE_PRINTF("\r\nRFID frame data CRC error");
					EVSE_PRINTF("\r\nEVSE Response NACK");
				}
			}
			else
			{
				df_buffer[0] = FRAME_EVSE_NACK;
				EVSE_PRINTF("\r\nRespon NACK");
			}
			// response ack/nack to evse
			frame_com_send(frame_instance, (uint8_t)FRAME_EVSE_UID, df_buffer, sizeof(df_buffer));
			break;
		}

		/* KEYA command */
		case FRAME_EVSE_KEYA:
			break;

		/* SECTOR command */
		case FRAME_EVSE_SECTOR:
			break;

		/* AES command */
		case FRAME_EVSE_AES:
			break;

		default:
			break;
		}
	}
}
