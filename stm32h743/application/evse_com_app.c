#include <string.h>
#include "ticker.h"
#include "frame.h"
#include "frame_com.h"
#include "evse_com_app.h"

#if (defined EVSE_COM_DEBUG_ENABLE) && (EVSE_COM_DEBUG_ENABLE == 1)
#include "serial_console_dbg.h"
#define EVSE_PRINTF(f_, ...)               PRINTF(f_, ##__VA_ARGS__)
#define EVSE_TAG_PRINTF(f_, ...)           do {\
                                               PRINTF("\r\n[EVSE] ");\
                                               PRINTF((f_), ##__VA_ARGS__);\
                                            } while(0)
#else
#define EVSE_PRINTF(f_, ...)
#define EVSE_TAG_PRINTF(f_, ...)
#endif

#define EVSE_UID_PRINTF(f_, ...)               PRINTF(f_, ##__VA_ARGS__)

#define FRAME_EVSE_BYTE_ACK	    0
#define FRAME_EVSE_BYTE_NACK	1

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  int command;
  void (*process_cb)(frame_com_cxt_t*, evse_command_t, uint8_t*, size_t);
} evse_cmd_handle_t;

/* Private define ------------------------------------------------------------*/
#define EVSE_CMD_HANDLE_NUM  (sizeof(evse_cmd_handle) / sizeof(evse_cmd_handle[0]))

/* Private variables ---------------------------------------------------------*/
/**@brief String literals for the serial command. */
#if (defined EVSE_COM_DEBUG_ENABLE) && (EVSE_COM_DEBUG_ENABLE == 1)
static char const * lit_command_id[] = {
/* 0 */ "FRAME_EVSE_HEART_BEAT",
/* 1 */ "FRAME_EVSE_RTC",
/* 2 */ "FRAME_EVSE_ERR_TYPE",
/* 3 */ "FRAME_EVSE_HW_VERSION",
/* 4 */ "FRAME_EVSE_FW_VERSION",
/* 5 */ "NC0",
/* 6 */ "NC1",
/* 7 */ "NC2",
/* 8 */ "NC3",
/* 9 */ "NC4",
/* 10*/ "NC5",
/* 11*/ "NC6",
/* 12*/ "NC7",
/* 13*/ "NC8",
/* 14*/ "NC9",
/* 15*/ "NC10",
/* 16*/ "FRAME_EVSE_JIG_TEST"
};

static char const* lit_frame_result_id[] =
{
  "FRAME_OK",
  "FRAME_ERR",
  "FRAME_SIZE_MIN_ERR",
  "FRAME_SOF_EOF_ERR",
  "FRAME_LENGTH_PACK_ERR",
  "FRAME_CRC_ERR"
};
#endif

/* Private function prototypes -----------------------------------------------*/

static evse_callback_handle_t* _callback;

static void make_ack_response(frame_com_cxt_t* fc, evse_command_t cmd);
static void make_nack_response(frame_com_cxt_t* fc, evse_command_t cmd);
static void heart_beat_cmd_receive(frame_com_cxt_t* fc, evse_command_t cmd, uint8_t* data, size_t length);
static void jig_test_mode_recieve(frame_com_cxt_t* fc, evse_command_t cmd, uint8_t* data, size_t length);

static evse_cmd_handle_t const evse_cmd_handle[] = {
    {FRAME_EVSE_HEART_BEAT, heart_beat_cmd_receive},
    {FRAME_EVSE_ERR_TYPE  , NULL},
    {FRAME_EVSE_HW_VERSION, NULL},
    {FRAME_EVSE_FW_VERSION, NULL},
    {FRAME_EVSE_JIG_TEST  , jig_test_mode_recieve}
};

void evse_callback_register(evse_callback_handle_t* cb)
{
  _callback = cb;
}

/* Brief: API send frame command to EVSE
 * [cmd]: command refer from rfid_command_t
 * [data]: data buffer shall fill to data's field of frame command
 * [length]: the length of data buffer
 * */
void evse_frame_transmit(frame_com_cxt_t* fc, uint8_t cmd, uint8_t* data, uint16_t length)
{
  if(FRAME_EVSE_NUM > cmd)
  {
    EVSE_PRINTF("\r\n[%u][ Send frame ]", fc->instance);
    EVSE_PRINTF("\r\n- CMD: %s", lit_command_id[cmd]);
    EVSE_PRINTF("\r\n- Data length: %u", length);
    EVSE_PRINTF("\r\n- Data: ");
    if (0 != length)
    {
      for(uint16_t i = 0; i < length; ++i)
      {
        EVSE_PRINTF("{%02X} ", data[i]);
      }
    }
    else
    {
      EVSE_PRINTF("NONE");
    }
    EVSE_PRINTF("\r\n");
    frame_com_transmit(fc, cmd, data, length);
  }
  else
  {
    EVSE_PRINTF("\r\n[ERROR] Unknown Command ");
  }
}

/* Brief: the function event callback parsed a frame message */
void evse_receive_cmd_callback(frame_com_cxt_t* fc, uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length)
{
  EVSE_PRINTF("\r\n[%u] Result Receive command: %s", fc->instance, lit_frame_result_id[result]);

  if((uint8_t)FRAME_OK == result)
	{
		if(FRAME_EVSE_NUM > cmd)
		{
		  evse_cmd_handle_t* p_cmd = (evse_cmd_handle_t*)evse_cmd_handle;
		  uint8_t cmd_handle = 0;

		  EVSE_PRINTF("\r\n[%u][ Get new frame ]", fc->instance);
      EVSE_PRINTF("\r\n- CMD: %s", lit_command_id[cmd]);
      EVSE_PRINTF("\r\n- Data length: %u", length);
      EVSE_PRINTF("\r\n- Data: ");
      for(uint16_t i = 0; i < length; ++i)
      {
        EVSE_PRINTF("{%02X} ", data[i]);
      }
      EVSE_PRINTF("\r\n");

      for (uint8_t i = 0; i < EVSE_CMD_HANDLE_NUM; ++i)
      {
        /* Find command handle */
        if (p_cmd[i].command == cmd)
        {
          /* Assert callback function */
          if (p_cmd[i].process_cb)
          {
            p_cmd[i].process_cb(fc, cmd, data, length);
          }
          else
          {
            EVSE_PRINTF("\r\nNone callback function handle");
          }

          cmd_handle = 1;
          break;
        } // if (p_cmd[i].command == cmd)
      } // For()

      if (!cmd_handle)
      {
        EVSE_PRINTF("\r\nCommand[%u] handle None", cmd);
      }
		}
		else
		{
		  EVSE_PRINTF("\r\n[%u] Unknown Command\r\n", fc->instance);
		}
	} // if((uint8_t)FRAME_OK == result)
}

static void make_ack_response(frame_com_cxt_t* fc, evse_command_t cmd)
{
  uint8_t data_buf[ACK_DF_LENGTH];
  data_buf[ACK_DF_ACK_INDEX] = FRAME_EVSE_BYTE_ACK;
  evse_frame_transmit(fc, (uint8_t)cmd, data_buf, ACK_DF_LENGTH);
  EVSE_PRINTF("\r\n[%u][%u] ACK response", fc->instance, lit_command_id[cmd]);
  EVSE_PRINTF("\r\n");
}

static void make_nack_response(frame_com_cxt_t* fc, evse_command_t cmd)
{
  uint8_t data_buf[ACK_DF_LENGTH];
  data_buf[ACK_DF_ACK_INDEX] = FRAME_EVSE_BYTE_NACK;
  evse_frame_transmit(fc, (uint8_t)cmd, data_buf, ACK_DF_LENGTH);
  EVSE_PRINTF("\r\n[%u][%u] NACK response", fc->instance, lit_command_id[cmd]);
  EVSE_PRINTF("\r\n");
}

/* RFID Handle receive heart beat command
 * Response ACK/NACK
 * HEART BEAT command
 * Hercules terminal test string: $7E$02$00$02$7F
 * Hex recieve:      {7E}{02}{00}{02}{7F}
 * Hex ACK response: {7E}{03}{00}{00}{03}{7F}
 * */
static void heart_beat_cmd_receive(frame_com_cxt_t* fc, evse_command_t cmd, uint8_t* data, size_t length)
{
  /* Assert data length heart beat receive command */
  if(0 == length)
  {
    EVSE_PRINTF("\r\n[%u][%s] command succeed", fc->instance, lit_command_id[cmd]);
    make_ack_response(fc, cmd);
  }
  else
  {
    EVSE_PRINTF("\r\n[%u][%s] length command failure", fc->instance, lit_command_id[cmd]);
    make_nack_response(fc, cmd);
  }
  EVSE_PRINTF("\r\n");
}

/* Command enter jig mode
 * Receive :$7E$06$14$01$02$03$04$16$7F
 * Response:$7E$03$14$00$17$7F
 * */
static void jig_test_mode_recieve(frame_com_cxt_t* fc, evse_command_t cmd, uint8_t* data, size_t length)
{
  uint8_t jig_data[4] = {0x01, 0x02, 0x03, 0x04};

  /* assert length */
  if (JIG_DF_LENGTH == length)
  {
    /* assert data */
    if (!memcmp(data, jig_data, JIG_DF_LENGTH))
    {
      EVSE_PRINTF("\r\n[%u][%s] command succeed", fc->instance, lit_command_id[cmd]);
      make_ack_response(fc, cmd);
      if (_callback->jig_setup_cb)
      {
        /* Jig enable */
        _callback->jig_setup_cb(1);
      }
    }
    else
    {
      EVSE_PRINTF("\r\n[%u][%s] data command failure", fc->instance, lit_command_id[cmd]);
      make_nack_response(fc, cmd);
    }
  }
  else
  {
    EVSE_PRINTF("\r\n[%u][%s] length command failure", fc->instance, lit_command_id[cmd]);
    make_nack_response(fc, cmd);
  }
}
