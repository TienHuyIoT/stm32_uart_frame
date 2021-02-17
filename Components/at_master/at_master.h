#ifndef __AT_MASTER_H
#define __AT_MASTER_H

#include <stdint.h>
#include "ticker.h"
#include "app_fifo_extra.h"

#define AT_MASTER_DEBUG    1

#define AT_FIFO_BUF_SIZE   128 /**< buffer size is a power of two */

typedef enum {
  AT_MASTER_OK = 0,
  AT_MASTER_ERROR,
  AT_MASTER_BUSY
} at_master_status_t;

typedef enum {
  AT_CMD_NONE = 0,
  AT_CMD_TEST,
  AT_CMD_QUERY,
  AT_CMD_SETUP,
  AT_CMD_EXE
} at_master_type_t;

typedef enum {
  AT_RESP_OK = 0,
  AT_RESP_ERROR,
  AT_RESP_TIMEOUT,
  AT_RESP_GARBAGE
} at_master_resp_t;

typedef enum {
  SM_AT_MASTER_PACK = 0,
  SM_AT_MASTER_RESP,
  SM_AT_MASTER_FINISH
} at_master_state_machine_t;

typedef struct
{
  struct {
    char *buf;
    size_t size;
  } at;

  struct {
    uint8_t *buf;
    size_t size;
  } fifo;
} at_master_buffer_t;

struct at_master_funcation;
typedef void (*response_cb)(char*, size_t, at_master_resp_t);
typedef size_t (*at_host_rx_cb)(uint8_t*, size_t);
typedef size_t (*at_host_tx_cb)(uint8_t*, size_t);

/**
 * @brief at_cmd_struct
 *  used for define at command
 *
 */
typedef struct at_master_funcation {
    char *cmd_name;                            /*!< at command name */
    char *resp_name;                           /*!< resp command name */
    void (*test_cmd)(struct at_master_funcation*);    /*!< Test Command function pointer AT+<CMD>=?\r */
    void (*query_cmd)(struct at_master_funcation*);   /*!< Query Command function pointer AT+<CMD>?\r */
    void (*setup_cmd)(struct at_master_funcation*, void *arg); /*!< Setup Command function pointer AT+<CMD>=<data>\r */
    void (*exe_cmd)(struct at_master_funcation*);     /*!< Execute Command function pointer AT+<CMD>\r */
    response_cb response;                      /*!< Execute response function */
    size_t timeout_resp;
    at_host_tx_cb write;                       /*!< Execute write to framework output */
    void* arg;
    at_master_type_t type;                     /*!< Type command */
} at_master_funcation_t;

typedef struct monitor_funcation {
    char *name;                            /*!< string monitor */
    uint8_t length;                        /*!< string length */
    uint8_t index;
    void (*exe)(struct monitor_funcation*, void *arg);
} monitor_funcation_t;

typedef struct
{
  at_host_rx_cb input_cb;
  at_host_tx_cb output_cb;
  app_fifo_t cmd_fifo;
  response_cb response;
  at_master_funcation_t* cmd_table;
  monitor_funcation_t* monitor_table;
  size_t monitor_num;
  ticker_t timeout;
  size_t size;        /* Size of buffer */
  char *buff;         /* Pointer to buffer process at command */
  char *resp_name;
  uint8_t resp_length;
  uint8_t at_ok_id;
  uint8_t at_error_id;
  uint8_t at_resp_id;
  uint8_t buff_id;
  uint8_t state_machine;
  int8_t cmd_index;
} at_master_cxt_t;

/* [AT_BUF_SIZE + 1] reserve NULL terminal*/
#define AT_MASTER_INIT(P_COMM_PARAMS, AT_BUF_SIZE) \
  do                                               \
  {                                                \
    at_master_buffer_t buffer;                     \
    static char buf[AT_BUF_SIZE + 1];              \
    static uint8_t fifo_buf[AT_FIFO_BUF_SIZE];     \
    buffer.at.buf = buf;                           \
    buffer.at.size = sizeof(buf);                  \
    buffer.fifo.buf = fifo_buf;                    \
    buffer.fifo.size = sizeof(fifo_buf);           \
    at_master_init(P_COMM_PARAMS, &buffer);        \
  } while (0)

at_master_status_t at_master_add_cmd(at_master_cxt_t *at, uint8_t cmd_index,
    void *cmd_arg, at_master_type_t cmd_type, response_cb cb);
void at_master_init(at_master_cxt_t *at, at_master_buffer_t *p_buffer);
void at_master_task(at_master_cxt_t* at);

#endif
