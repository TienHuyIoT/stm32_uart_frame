#ifndef __AT_CMD_PARSE_H
#define __AT_CMD_PARSE_H

#define AT_MASTER_HANDLE_DEBUG 1
#define HEAP_SIGNAL_VALUE       0xAA
#define HEAP_SIGNAL_RESET       0

#include <stdint.h>
#include "at_master.h"

typedef enum {
    AT_CMD = 0,
    SWV_CMD,
    HWV_CMD,
    SN_CMD,
    JIG_CMD
} at_command_list;

typedef struct {
    uint8_t rsp_flag;
    void* data;
} at_response;

typedef union {
    struct {
        uint8_t major;
        uint8_t minor;
        uint16_t build;
        uint8_t heap_sign;
    } swv;

    struct {
        char* str;
        uint8_t heap_sign;
    } hwv;

    struct {
        char* str;
        uint8_t heap_sign;
    } sn;

    struct {
        uint8_t cmd;
        uint8_t heap_sign;
    } jig;
} at_cmd_arg;

extern const size_t MONITOR_LIST_NUM;

extern at_master_funcation_t at_fun_list[];
extern monitor_funcation_t monitor_list[];

#if (defined AT_MASTER_HANDLE_DEBUG) && (AT_MASTER_HANDLE_DEBUG == 1)
void at_master_handle_test(at_master_cxt_t* at);
#endif

#endif
