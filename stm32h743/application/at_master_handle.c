#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_config.h"
#include "Tools.h"
#include "at_master_handle.h"

#if (defined AT_MASTER_HANDLE_DEBUG) && (AT_MASTER_HANDLE_DEBUG == 1)
#include "serial_console_dbg.h"
#define APP_AT_MASTER_PRINT(f_, ...)     DBG_PRINTF(f_, ##__VA_ARGS__)
#define APP_AT_MASTER_TAG_PRINT(f_, ...) do {\
                                          DBG_PRINTF("\r\n[AT_PARSE] ");\
                                          DBG_PRINTF((f_), ##__VA_ARGS__);\
                                          DBG_PRINTF("\n");\
                                        } while(0)
#else
#define APP_AT_MASTER_PRINT(f_, ...)
#define APP_AT_MASTER_TAG_PRINT(f_, ...)
#endif

#define RESP_TIMEOUT_MS    2000

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define AT_MASTER_PACK(W, f_, ...)    vspfunc(W, (f_), ##__VA_ARGS__)

/* Private variables ---------------------------------------------------------*/
static const char* at_header = "AT";

/* Private function prototypes -----------------------------------------------*/
static void at_exe_pack_null(at_master_funcation_t *at);
static void at_resp_null(char* data, size_t len, at_master_resp_t result);

static void at_query_pack_jig(at_master_funcation_t *at);
static void at_setup_pack_jig(at_master_funcation_t *at, void *arg);
static void at_resp_jig(char* data, size_t len, at_master_resp_t result);

static void low_power_alert(monitor_funcation_t* mf, void *arg);
static void high_power_alert(monitor_funcation_t* mf, void *arg);

/* cmd_name, resp_name, (*test_cmd) , (*query_cmd), (*setup_cmd) , (*exe_cmd), (*response), timeout_resp
 *         ,          , AT+<CMD>=?\r, AT+<CMD>?\r , AT+<CMD>=..\r, AT+<CMD>\r */
at_master_funcation_t at_fun_list[] = {
  {""       , NULL, NULL, NULL, NULL, at_exe_pack_null, at_resp_null, RESP_TIMEOUT_MS},
  {"+SWV"   , NULL, NULL, NULL, NULL, NULL, NULL, RESP_TIMEOUT_MS},
  {"+HWV"   , NULL, NULL, NULL, NULL, NULL, NULL, RESP_TIMEOUT_MS},
  {"+SN"    , NULL, NULL, NULL, NULL, NULL, NULL, RESP_TIMEOUT_MS},
  {"+JIG"   , NULL, NULL, at_query_pack_jig, at_setup_pack_jig, NULL, at_resp_jig, RESP_TIMEOUT_MS}
};

/* name, length, index, (*exe) */
monitor_funcation_t monitor_list[] = {
  {"\r\nLOW POWER\r\n"  , 13, 0, low_power_alert},
  {"\r\nHIGH POWER\r\n" , 14, 0, high_power_alert}
};

const size_t MONITOR_LIST_NUM = (sizeof(monitor_list) / sizeof(monitor_list[0]));

#if (defined AT_MASTER_HANDLE_DEBUG) && (AT_MASTER_HANDLE_DEBUG == 1)
static void at_resp_jig_cb(char* data, size_t len, at_master_resp_t result)
{
  if(AT_RESP_OK == result)
  {
    APP_AT_MASTER_TAG_PRINT("at_resp_jig_cb");
  }
}

void at_master_handle_test(at_master_cxt_t* at)
{
  at_cmd_arg* jig_setup_arg;

  // Test at command
  at_master_push_cmd(at, AT_CMD, NULL, AT_CMD_EXE, NULL);

  // Test jig query command with callback register
  at_master_push_cmd(at, JIG_CMD, NULL, AT_CMD_QUERY, at_resp_jig_cb);

  // Test jig setup command
  jig_setup_arg = (at_cmd_arg*)malloc(sizeof(at_cmd_arg));
  if(jig_setup_arg != NULL)
  {
    APP_AT_MASTER_TAG_PRINT("Allocate %u-Byte arg succeed location at %lu", sizeof(at_cmd_arg), (uint32_t)jig_setup_arg);
    jig_setup_arg->jig.cmd = 1;
    jig_setup_arg->jig.heap_sign = HEAP_SIGNAL_VALUE;
    at_master_push_cmd(at, JIG_CMD, (void*)jig_setup_arg, AT_CMD_SETUP, NULL);
  }
}
#endif

static void at_exe_pack_null(at_master_funcation_t *at)
{
  AT_MASTER_PACK(at->write, "%s\r", at_header);
}

static void at_resp_null(char* data, size_t len, at_master_resp_t result)
{
  if(AT_RESP_OK == result)
  {
    APP_AT_MASTER_TAG_PRINT("at_resp_null");
  }
}

static void at_query_pack_jig(at_master_funcation_t *at)
{
  AT_MASTER_PACK(at->write, "%s%s?\r", at_header, at->cmd_name);
}

static void at_setup_pack_jig(at_master_funcation_t *at, void *arg)
{
  if(arg != NULL)
  {
    at_cmd_arg* setup_arg = (at_cmd_arg*)arg;
    AT_MASTER_PACK(at->write, "%s%s=%u\r", at_header, at->cmd_name, setup_arg->jig.cmd);
    if(HEAP_SIGNAL_VALUE == setup_arg->jig.heap_sign)
    {
      APP_AT_MASTER_TAG_PRINT("Free arg location at %lu",(uint32_t)arg);
      setup_arg->jig.heap_sign = HEAP_SIGNAL_RESET;
      free(arg);
    }
  }
  else
  {
    APP_AT_MASTER_TAG_PRINT("at_setup_pack_jig FAIL: Arg NULL");
  }
}

static void at_resp_jig(char* data, size_t len, at_master_resp_t result)
{
  if(AT_RESP_OK == result)
  {
    APP_AT_MASTER_TAG_PRINT("at_resp_jig");
  }
}

static void low_power_alert(monitor_funcation_t* mf, void *arg)
{
  APP_AT_MASTER_TAG_PRINT("Monitor cb [%s]", mf->name);
}

static void high_power_alert(monitor_funcation_t* mf, void *arg)
{
  APP_AT_MASTER_TAG_PRINT("Monitor cb [%s]", mf->name);
}
