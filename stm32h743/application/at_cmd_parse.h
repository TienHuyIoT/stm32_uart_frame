#ifndef __AT_CMD_PARSE_H
#define __AT_CMD_PARSE_H

#define AT_CMD_PARSE_DEBUG 1

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
    } swv;

    struct {
        char* str;
    } hwv;

    struct {
        char* str;
    } sn;

    struct {
        uint8_t cmd;
    } jig;
} at_cmd_arg;

//typedef struct {
//    int8_t (*pack)(at_cmd_arg* cmd_arg);
//    void (*response)(char* data, size_t len);
//} at_cmd_func_t;

extern const size_t MONITOR_LIST_NUM;

extern at_master_funcation_t at_fun_list[];
extern monitor_funcation_t monitor_list[];

#endif
