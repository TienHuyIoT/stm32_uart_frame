#ifndef __EVSE_COM_APP_H
#define __EVSE_COM_APP_H

#include <stdint.h>

#define EVSE_COM_DEBUG_ENABLE	1

typedef enum
{
/* 0 */ FRAME_EVSE_HEART_BEAT = 0,
/* 1 */ FRAME_EVSE_UID,
/* 2 */ FRAME_EVSE_KEYA,
/* 3 */ FRAME_EVSE_SECTOR,
/* 4 */ FRAME_EVSE_AES,
/* 5 */ FRAME_EVSE_RTC,
/* 6 */ FRAME_EVSE_NUM
} evse_command_t;

/* Uid data frame element index*/
#define UID_DF_LENGTH				16 /* number of data field */
#define UID_DF_UID_INDEX            0  /* length is 7 byte */
#define UID_DF_UID_LENGTH_INDEX     7  /* length is 1 byte */
#define UID_DF_RESERVE_INDEX        8  /* length is 5 byte */
#define UID_DF_TIMESTAMP_INDEX      13 /* length is 2 byte */
#define UID_DF_CRC_INDEX            15 /* length is 1 byte */

typedef struct {
	int (*input_cb)(uint8_t *c);
	int (*output_cb)(uint8_t c);
	void (*uid_cb)(uint8_t* uid, uint8_t length);
} rfid_frame_handle_t;

void evse_handle_init(rfid_frame_handle_t *fp_callback);
void evse_handle_loop(void);
void evse_uid_callback(uint8_t* uid, uint8_t length);

#endif
