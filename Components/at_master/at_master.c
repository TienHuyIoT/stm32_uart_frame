#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "ticker.h"
#include "Tools.h"
#include "at_master.h"

#if (defined AT_MASTER_DEBUG) && (AT_MASTER_DEBUG == 1)
#include "serial_console_dbg.h"
#define AT_HOST_PRINT(f_, ...)          DBG_PRINTF(f_, ##__VA_ARGS__)
#define AT_HOST_TAG_PRINT(f_, ...)      do {\
                                           DBG_PRINTF("\r\n[AT_MASTER] ");\
                                           DBG_PRINTF((f_), ##__VA_ARGS__);\
                                           DBG_PRINTF("\n");\
                                        } while(0)
#else
#define AT_HOST_PRINT(f_, ...)
#define AT_HOST_TAG_PRINT(f_, ...)
#endif

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  void *cmd_arg;
  response_cb cb;
  uint8_t cmd_index;
  uint8_t cmd_type;
} at_master_cmd_handle_t;

/* Private define ------------------------------------------------------------*/
#define CHAR_RESP_LIMIT    30
#define SM_ACTIVE(x, y)    (x = y)

/* Private variables ---------------------------------------------------------*/
#if (defined AT_MASTER_DEBUG) && (AT_MASTER_DEBUG == 1)
/**@brief String literals for the frame parse. */
static char const *lit_at_master_type[] =
  {
    "AT_CMD_NONE",
    "AT_CMD_TEST",
    "AT_CMD_QUERY",
    "AT_CMD_SETUP",
    "AT_CMD_EXE"};

static char const *lit_at_master_resp[] =
  {
    "AT_RESP_OK",
    "AT_RESP_ERROR",
    "AT_RESP_TIMEOUT",
    "AT_RESP_GARBAGE"};

static char const *lit_at_master_status[] =
  {
    "AT_MASTER_OK",
    "AT_MASTER_ERROR",
    "AT_MASTER_BUSY"};
#endif

const char at_master_rec_ok[]        = "\r\nOK\r\n";
const char at_master_rec_error[]     = "\r\nERROR\r\n";
const char AT_MASTER_OK_LENGTH       =  sizeof(at_master_rec_ok) - 1;
const char AT_MASTER_ERROR_LENGTH    =  sizeof(at_master_rec_error) - 1;

static int8_t get_cmd_index(at_master_cxt_t *at);
static void _empty_input(at_host_rx_cb rx);

/* Brief: Add new command into fifo */
at_master_status_t at_master_push_cmd(at_master_cxt_t *at, uint8_t cmd_index,
    void *cmd_arg, at_master_type_t cmd_type, response_cb cb)
{
  uint32_t buff_lenght = sizeof(at_master_cmd_handle_t);
  uint32_t ff_length = app_fifo_available(&at->cmd_fifo);
  uint8_t status;
  AT_HOST_TAG_PRINT("Fifo available %lu, buff length %lu", ff_length, buff_lenght);

  if (ff_length >= buff_lenght)
  {
    at_master_cmd_handle_t cmd_handle;
    cmd_handle.cb = cb;
    cmd_handle.cmd_arg = cmd_arg;
    cmd_handle.cmd_index = cmd_index;
    cmd_handle.cmd_type = cmd_type;
    app_fifo_write(&at->cmd_fifo, (uint8_t*)&cmd_handle, &buff_lenght);
    status = AT_MASTER_OK;
  }
  else
  {
    status = AT_MASTER_BUSY;
  }

  AT_HOST_TAG_PRINT("Push cmd %s", lit_at_master_status[status]);
  return status;
}

static void _empty_input(at_host_rx_cb rx)
{
  uint8_t c;
  while (rx(&c, 1)) {};
}

static int8_t get_cmd_index(at_master_cxt_t *at)
{
  int8_t index = -1;

  if (app_fifo_length(&at->cmd_fifo) > 0)
  {
    uint32_t buff_lenght = sizeof(at_master_cmd_handle_t);
    at_master_funcation_t *p_cmd_handle;
    at_master_cmd_handle_t cmd_handle;
    app_fifo_read(&at->cmd_fifo, (uint8_t*)&cmd_handle, &buff_lenght);
    if(sizeof(at_master_cmd_handle_t) == buff_lenght)
    {
      at->cmd_index = cmd_handle.cmd_index;
      p_cmd_handle = (at_master_funcation_t*) &at->cmd_table[at->cmd_index];
      p_cmd_handle->type = cmd_handle.cmd_type;
      p_cmd_handle->arg = cmd_handle.cmd_arg;
      at->response = cmd_handle.cb;
      index = at->cmd_index;
      AT_HOST_TAG_PRINT("Get cmd index %d", index);
      if (cmd_handle.cb)
      {
        AT_HOST_TAG_PRINT("Register cb %lu", (uint32_t)cmd_handle.cb);
      }
    }
    AT_HOST_TAG_PRINT("Get %lu bytes, fifo available %lu", buff_lenght, app_fifo_available(&at->cmd_fifo));
  }

  return index;
}

void at_master_init(at_master_cxt_t *at, at_master_buffer_t *p_buffer)
{
  at->at_ok_id = 0;
  at->at_error_id = 0;
  at->at_resp_id = 0;
  at->cmd_index = -1;
  at->buff_id = 0;
  at->buff = p_buffer->at.buf;
  at->size = p_buffer->at.size;
  // Configure cmd fifo buffer.
  app_fifo_init(&at->cmd_fifo, p_buffer->fifo.buf, p_buffer->fifo.size);
  at->response = NULL;
  ticker_stop(&at->timeout);
}

void at_master_handle(at_master_cxt_t* at)
{
  switch (at->state_machine)
  {
  case SM_AT_MASTER_PACK:
    if(get_cmd_index(at) != -1)
    {
      at_master_funcation_t* p_cmd_handle = (at_master_funcation_t*)&at->cmd_table[at->cmd_index];
      at->resp_name = p_cmd_handle->resp_name;
      at->resp_length = strlen(at->resp_name);
      /* Add write pointer to output callback */
      p_cmd_handle->write = at->output_cb;
      /* Empty input buffer */
      _empty_input(at->input_cb);
      AT_HOST_TAG_PRINT("Active id %d, type %s, timeout %u", at->cmd_index,
          lit_at_master_type[p_cmd_handle->type], p_cmd_handle->timeout_resp);
      if(AT_CMD_TEST == p_cmd_handle->type)
      {
        if(p_cmd_handle->test_cmd)
        {
          p_cmd_handle->test_cmd(p_cmd_handle);
          ticker_begin(&at->timeout, p_cmd_handle->timeout_resp);
          SM_ACTIVE(at->state_machine, SM_AT_MASTER_RESP);
        }
      }
      else if(AT_CMD_QUERY == p_cmd_handle->type)
      {
        if(p_cmd_handle->query_cmd)
        {
          p_cmd_handle->query_cmd(p_cmd_handle);
          ticker_begin(&at->timeout, p_cmd_handle->timeout_resp);
          SM_ACTIVE(at->state_machine, SM_AT_MASTER_RESP);
        }
      }
      else if(AT_CMD_SETUP == p_cmd_handle->type)
      {
        if(p_cmd_handle->setup_cmd)
        {
          p_cmd_handle->setup_cmd(p_cmd_handle, p_cmd_handle->arg);
          ticker_begin(&at->timeout, p_cmd_handle->timeout_resp);
          SM_ACTIVE(at->state_machine, SM_AT_MASTER_RESP);
        }
      }
      else if(AT_CMD_EXE == p_cmd_handle->type)
      {
        if(p_cmd_handle->exe_cmd)
        {
          p_cmd_handle->exe_cmd(p_cmd_handle);
          ticker_begin(&at->timeout, p_cmd_handle->timeout_resp);
          SM_ACTIVE(at->state_machine, SM_AT_MASTER_RESP);
        }
      }
      else
      {
        SM_ACTIVE(at->state_machine, SM_AT_MASTER_FINISH);
      }
    }
    else
    {
      uint8_t buf[1]; /* Purpose the address is 4-byte aligned here */
      size_t cnt_limit;
      cnt_limit = CHAR_RESP_LIMIT;
      while (at->input_cb(buf, 1))
      {
        char c = buf[0];
        for(int i = 0; i < at->monitor_num; ++i)
        {
          monitor_funcation_t* p_monitor = (monitor_funcation_t*)&at->monitor_table[i];
          if (check_strncmp(p_monitor->name, c, &p_monitor->index, p_monitor->length))
          {
            AT_HOST_TAG_PRINT("Monitor cb %u", i);
            AT_HOST_PRINT("\r\n");
            p_monitor->exe(p_monitor, NULL);
          }
        }

        // Avoid while loop forever
        --cnt_limit;
        if(0 == cnt_limit) {
          break;
        }
      }
    }
    break;

  case SM_AT_MASTER_RESP:
  {
    uint8_t buf[1]; /* Purpose the address is 4-byte aligned here */
    size_t cnt_limit;
    uint8_t status = false;
    uint8_t result;

    if (ticker_expried(&at->timeout))
    {
      status = true;
      result = AT_RESP_TIMEOUT;
    }

    cnt_limit = CHAR_RESP_LIMIT;
    while (at->input_cb(buf, 1))
    {
      char c = buf[0];

      at->buff[at->buff_id++] = c;
      if (at->size == at->buff_id)
      {
        status = true;
        result = AT_RESP_GARBAGE;
      }

      if (check_strncmp((char*) at_master_rec_ok, c, &at->at_ok_id,
          AT_MASTER_OK_LENGTH))
      {
        status = true;
        result = AT_RESP_OK;
      }

      if (check_strncmp((char*) at_master_rec_error, c, &at->at_error_id,
          AT_MASTER_ERROR_LENGTH))
      {
        status = true;
        result = AT_RESP_ERROR;
      }

      if (at->resp_name)
      {
        if (check_strncmp(at->resp_name, c, &at->at_resp_id, at->resp_length))
        {
          status = true;
          result = AT_RESP_OK;
        }
      }

      for (int i = 0; i < at->monitor_num; ++i)
      {
        monitor_funcation_t *p_monitor =
            (monitor_funcation_t*) &at->monitor_table[i];
        if (check_strncmp(p_monitor->name, c, &p_monitor->index,
            p_monitor->length))
        {
          AT_HOST_TAG_PRINT("Monitor cb %u", i);
          AT_HOST_PRINT("\r\n");
          p_monitor->exe(p_monitor, NULL);
        }
      }

      if(status)
      {
        break;
      }

      // Avoid while loop forever
      --cnt_limit;
      if (0 == cnt_limit)
      {
        break;
      }

    } // while (at->input_cb(buf, 1))

    if (status)
    {
      if(AT_RESP_OK == result)
      {
        AT_HOST_TAG_PRINT("Data: ");
        for(int i = 0; i < at->buff_id; ++i)
        {
          AT_HOST_PRINT("%c", at->buff[i]);
        }
        AT_HOST_PRINT("\r\n");
      }

      at_master_funcation_t *p_cmd_handle =
          (at_master_funcation_t*) &at->cmd_table[at->cmd_index];
      if (p_cmd_handle->response)
      {
        AT_HOST_TAG_PRINT("Response index cb %d, result %s", at->cmd_index,
            lit_at_master_resp[result]);

        p_cmd_handle->response(at->buff, at->buff_id,
            (at_master_resp_t) result);
      }

      if (at->response)
      {
        AT_HOST_TAG_PRINT("Response register cb %lu, result %s", (uint32_t)at->response,
            lit_at_master_resp[result]);

        at->response(at->buff, at->buff_id,
            (at_master_resp_t) result);
        AT_HOST_TAG_PRINT("Delete registered cb");
        at->response = NULL;
      }

      SM_ACTIVE(at->state_machine, SM_AT_MASTER_FINISH);
    }
    break;
  }

  case SM_AT_MASTER_FINISH:
    at->at_ok_id = 0;
    at->at_error_id = 0;
    at->at_resp_id = 0;
    at->cmd_index = -1;
    at->buff_id = 0;
    at->response = NULL;
    ticker_stop(&at->timeout);
    SM_ACTIVE(at->state_machine, SM_AT_MASTER_PACK);
    AT_HOST_TAG_PRINT("Restart Process");
    break;
  }
}
