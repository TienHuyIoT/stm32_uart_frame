#ifndef __EVSE_COM_APP_H
#define __EVSE_COM_APP_H

#include <stdint.h>
#include "frame_com.h"

#define EVSE_COM_DEBUG_ENABLE	   0

typedef enum
{
  EVSE_UART_INTERFACE = 0,
  EVSE_RS232_INTERFACE
} evse_frame_instance_t;

typedef enum
{
/* 0 */ FRAME_EVSE_HEART_BEAT = 0,
/* 1 */ FRAME_EVSE_RTC,
/* 2 */ FRAME_EVSE_ERR_TYPE,
/* 3 */ FRAME_EVSE_HW_VERSION,
/* 4 */ FRAME_EVSE_FW_VERSION,
/* 5 */ NC0,
/* 6 */ NC1,
/* 7 */ NC2,
/* 8 */ NC3,
/* 9 */ NC4,
/* 10*/ NC5,
/* 11*/ NC6,
/* 12*/ NC7,
/* 13*/ NC8,
/* 14*/ NC9,
/* 15*/ NC10,
/* 16*/ FRAME_EVSE_JIG_TEST,
/* 17*/ FRAME_EVSE_NUM
} evse_command_t;

/* Uid data frame element index*/
#define UID_DF_LENGTH				16 /* number of data field */
#define UID_DF_UID_INDEX            0  /* length is 7 byte */
#define UID_DF_UID_LENGTH_INDEX     7  /* length is 1 byte */
#define UID_DF_RESERVE_INDEX        8  /* length is 5 byte */
#define UID_DF_TIMESTAMP_INDEX      13 /* length is 2 byte */
#define UID_DF_CRC_INDEX            15 /* length is 1 byte */
#define UID_LENGTH_MAX				7  /* UID max length */

/* Uid data frame element index*/
#define JIG_DF_LENGTH               4 /* number of data field */

/* ACK data frame element*/
#define ACK_DF_LENGTH               1 /* number of data field */
#define ACK_DF_ACK_INDEX            0  /* length is 1 byte */

typedef struct {
  uint8_t (*jig_query_cb)(void);
  void (*jig_setup_cb)(uint8_t);
} evse_callback_handle_t;

void evse_callback_register(evse_callback_handle_t*);
void evse_frame_transmit(frame_com_cxt_t* fc, uint8_t cmd, uint8_t* data, uint16_t length);
void evse_receive_cmd_callback(frame_com_cxt_t* frame_instance, uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length);

#endif
