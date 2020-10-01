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

typedef struct {
	void (*uid)(uint8_t* uid, uint8_t length);
} rfid_frame_cb_t;

void evse_handle_init(rfid_frame_cb_t *fp_callback);
void evse_handle_loop(void);
void evse_uid_callback(uint8_t* uid, uint8_t length);

#endif
