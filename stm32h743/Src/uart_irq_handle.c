#include "ticker.h"
#include "serial_common.h"
#include "serial_console_dbg.h"
#include "serial1.h"
#include "serial2.h"
#include "serial3.h"
#include "serial4.h"
#include "serial5.h"
#include "serial6.h"
#include "serial7.h"
#include "serial8.h"
#include "main.h"

#define UART_ERROR_TICK_NUM_MS    300 /* ms */
#define UART_ERROR_COUNT_DOWN     2   /* Minimum is 2 */
#define UART_IDLE_IRQ_ENABLE(p_instance)   __HAL_UART_ENABLE_IT(p_instance, UART_IT_IDLE);

/* Private variables ---------------------------------------------------------*/
ticker_function_handle_t uart_error_tick_handle;
static volatile uint8_t uart_error[UART_INSTANCE_NUM] = {0};

#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
extern UART_HandleTypeDef huart1;
static app_uart_fifo_ctx_t  uart_instance1;
static void uart_instance1_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance1_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
extern UART_HandleTypeDef huart2;
static app_uart_fifo_ctx_t  uart_instance2;
static void uart_instance2_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance2_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
extern UART_HandleTypeDef huart3;
static app_uart_fifo_ctx_t  uart_instance3;
static void uart_instance3_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance3_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
extern UART_HandleTypeDef huart4;
static app_uart_fifo_ctx_t  uart_instance4;
static void uart_instance4_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance4_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
extern UART_HandleTypeDef huart5;
static app_uart_fifo_ctx_t  uart_instance5;
static void uart_instance5_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance5_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
extern UART_HandleTypeDef huart6;
static app_uart_fifo_ctx_t  uart_instance6;
static void uart_instance6_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance6_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
extern UART_HandleTypeDef huart7;
static app_uart_fifo_ctx_t  uart_instance7;
static void uart_instance7_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance7_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
extern UART_HandleTypeDef huart8;
static app_uart_fifo_ctx_t  uart_instance8;
static void uart_instance8_transmit_start_cb(uint8_t *p_data, uint32_t lenght);
static void uart_instance8_receive_start_cb(uint8_t *p_data, uint32_t lenght);
#endif

/* Private function prototypes -----------------------------------------------*/
static void uart_instance_event_handle(struct app_uart_fifo_ctx *p_app_cxt, app_uart_evt_t *p_event);
void uart_error_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event);
void uart_tx_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event);
void uart_rx_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event);

/* Private user code ---------------------------------------------------------*/
static void uart_error_handle_callback(uint32_t tick_remain)
{
	if(0) {}
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	else if (uart_error[UART_INSTANCE1] > 0)
	{
		--uart_error[UART_INSTANCE1];
		if (0 == uart_error[UART_INSTANCE1])
		{
			DBG_PRINTF("\r\nReinit uart1");
			uart_instance1_Init(&uart_instance1);
		}
	}
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	else if (uart_error[UART_INSTANCE2] > 0)
	{
		--uart_error[UART_INSTANCE2];
		if (0 == uart_error[UART_INSTANCE2])
		{
			DBG_PRINTF("\r\nReinit uart2");
			uart_instance2_Init(&uart_instance2);
		}
	}
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	else if (uart_error[UART_INSTANCE3] > 0)
	{
		--uart_error[UART_INSTANCE3];
		if (0 == uart_error[UART_INSTANCE3])
		{
			uart_instance3_Init(&uart_instance3);
		}
	}
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	else if (uart_error[UART_INSTANCE4] > 0)
	{
		--uart_error[UART_INSTANCE4];
		if (0 == uart_error[UART_INSTANCE4])
		{
			uart_instance4_Init(&uart_instance4);
		}
	}
#endif

#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
	else if (uart_error[UART_INSTANCE5] > 0)
	{
		--uart_error[UART_INSTANCE5];
		if (0 == uart_error[UART_INSTANCE5])
		{
			uart_instance5_Init(&uart_instance5);
		}
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	else if (uart_error[UART_INSTANCE6] > 0)
	{
		--uart_error[UART_INSTANCE6];
		if (0 == uart_error[UART_INSTANCE6])
		{
			DBG_PRINTF("\r\nReinit uart6\r\n");
			DBG_PRINTF("\r\n");
			uart_instance6_Init(&uart_instance6);
		}
	}
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	else if (uart_error[UART_INSTANCE7] > 0)
	{
		--uart_error[UART_INSTANCE7];
		if (0 == uart_error[UART_INSTANCE7])
		{
			uart_instance7_Init(&uart_instance7);
		}
	}
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	else if (uart_error[UART_INSTANCE8] > 0)
	{
		--uart_error[UART_INSTANCE8];
		if (0 == uart_error[UART_INSTANCE8])
		{
			uart_instance8_Init(&uart_instance8);
		}
	}
#endif

	else {}
}

void uart_error_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event)
{
	if(0) {}
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	else if (huart->Instance == huart1.Instance)
	{
		uart_instance1.irq_callback(&uart_instance1, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	else if (huart->Instance == huart2.Instance)
	{
		uart_instance2.irq_callback(&uart_instance2, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	else if (huart->Instance == huart3.Instance)
	{
		uart_instance3.irq_callback(&uart_instance3, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	else if (huart->Instance == huart4.Instance)
	{
		uart_instance4.irq_callback(&uart_instance4, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_5 == 1)
	else if (huart->Instance == huart5.Instance)
	{
		uart_instance5.irq_callback(&uart_instance5, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	else if (huart->Instance == huart6.Instance)
	{
		uart_instance6.irq_callback(&uart_instance6, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	else if (huart->Instance == huart7.Instance)
	{
		uart_instance7.irq_callback(&uart_instance7, APP_UART_IRQ_ERROR);
	}
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	else if (huart->Instance == huart8.Instance)
	{
		uart_instance8.irq_callback(&uart_instance8, APP_UART_IRQ_ERROR);
	}
#endif
	else {}
}

void uart_tx_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event)
{
	if(0) {}
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	else if (huart->Instance == huart1.Instance)
	{
		uart_instance1.irq_callback(&uart_instance1, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	else if (huart->Instance == huart2.Instance)
	{
		uart_instance2.irq_callback(&uart_instance2, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	else if (huart->Instance == huart3.Instance)
	{
		uart_instance3.irq_callback(&uart_instance3, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	else if (huart->Instance == huart4.Instance)
	{
		uart_instance4.irq_callback(&uart_instance4, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_5 == 1)
	else if (huart->Instance == huart5.Instance)
	{
		uart_instance5.irq_callback(&uart_instance5, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	else if (huart->Instance == huart6.Instance)
	{
		uart_instance6.irq_callback(&uart_instance6, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	else if (huart->Instance == huart7.Instance)
	{
		uart_instance7.irq_callback(&uart_instance7, irq_event);
	}
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	else if (huart->Instance == huart8.Instance)
	{
		uart_instance8.irq_callback(&uart_instance8, irq_event);
	}
#endif
	else {}
}

void uart_rx_irq_process(UART_HandleTypeDef *huart, app_uart_irq_t irq_event)
{
	if(0) {}
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	else if (huart->Instance == huart1.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance1.rx_irq.head = uart_instance1.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance1.irq_callback(&uart_instance1, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance1.rx_irq.head = uart_instance1.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance1.irq_callback(&uart_instance1, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	else if (huart->Instance == huart2.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance2.rx_irq.head = uart_instance2.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance2.irq_callback(&uart_instance2, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance2.rx_irq.head = uart_instance2.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance2.irq_callback(&uart_instance2, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	else if (huart->Instance == huart3.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance3.rx_irq.head = uart_instance3.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance3.irq_callback(&uart_instance3, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance3.rx_irq.head = uart_instance3.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance3.irq_callback(&uart_instance3, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	else if (huart->Instance == huart4.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance4.rx_irq.head = uart_instance4.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance4.irq_callback(&uart_instance4, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance4.rx_irq.head = uart_instance4.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance4.irq_callback(&uart_instance4, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
	else if (huart->Instance == huart5.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance5.rx_irq.head = uart_instance5.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance5.irq_callback(&uart_instance5, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance5.rx_irq.head = uart_instance5.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance5.irq_callback(&uart_instance5, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	else if (huart->Instance == huart6.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance6.rx_irq.head = uart_instance6.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance6.irq_callback(&uart_instance6, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance6.rx_irq.head = uart_instance6.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance6.irq_callback(&uart_instance6, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	else if (huart->Instance == huart7.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance7.rx_irq.head = uart_instance7.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance7.irq_callback(&uart_instance7, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance7.rx_irq.head = uart_instance7.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance7.irq_callback(&uart_instance7, irq_event);
#endif
	}
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	else if (huart->Instance == huart8.Instance)
	{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
		/* Get head position of rx_irq buffer
		 * __HAL_DMA_GET_COUNTER() is the macro return number of remaining data units in the current DMA Stream transfer */
		uart_instance8.rx_irq.head = uart_instance8.rx_irq.xfer_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		uart_instance8.irq_callback(&uart_instance8, irq_event);
#else
		/* Get head position of rx_irq buffer
		 * huart->RxXferCount shall return number of remaining data units in the current rx_irq buffer */
		uart_instance8.rx_irq.head = uart_instance8.rx_irq.xfer_size - huart->RxXferCount;
		uart_instance8.irq_callback(&uart_instance8, irq_event);
#endif
	}
#endif
	else {}
}

#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance1_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart1, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart1, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance1_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart1, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart1, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance2_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart2, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart2, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance2_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart2, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart2, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance3_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart3, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart3, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance3_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart3, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart3, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance4_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart4, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart4, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance4_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart4, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart4, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance5_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart5, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart5, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance5_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart5, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart5, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance6_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart6, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart6, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance6_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart6, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart6, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance7_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart7, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart7, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance7_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart7, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart7, p_data, lenght);
#endif
}
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
/* callback uart transmit start
 * Using DMA Tx
 * */
static void uart_instance8_transmit_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_TX_DMA) && (APP_UART_FIFO_TX_DMA == 1)
	HAL_UART_Transmit_DMA(&huart8, p_data, lenght);
#else
	HAL_UART_Transmit_IT(&huart8, p_data, lenght);
#endif
}

/* callback uart receive start
 * Using Interrupt rx
 * */
static void uart_instance8_receive_start_cb(uint8_t *p_data, uint32_t lenght)
{
#if (defined APP_UART_FIFO_RX_DMA) && (APP_UART_FIFO_RX_DMA == 1)
	HAL_UART_Receive_DMA(&huart8, p_data, lenght);
#else
	HAL_UART_Receive_IT(&huart8, p_data, lenght);
#endif
}
#endif

void uart_irq_init(void)
{
	/* Init tick handle uart error with interval UART_ERROR_TICK_NUM_MS */
	ticker_function_init(&uart_error_tick_handle, uart_error_handle_callback,
			UART_ERROR_TICK_NUM_MS, TICKER_FOREVER);

	/* Init object app_uart_fifo */
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	uart_instance1.instance    = UART_INSTANCE1;
	uart_instance1.fp_transmit = uart_instance1_transmit_start_cb;
	uart_instance1.fp_receive  = uart_instance1_receive_start_cb;
	uart_instance1.fp_event    = uart_instance_event_handle;
	uart_instance1_Init(&uart_instance1);

	UART_IDLE_IRQ_ENABLE(&huart1);
#endif

#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	uart_instance2.instance    = UART_INSTANCE2;
	uart_instance2.fp_transmit = uart_instance2_transmit_start_cb;
	uart_instance2.fp_receive  = uart_instance2_receive_start_cb;
	uart_instance2.fp_event    = uart_instance_event_handle;
	uart_instance2_Init(&uart_instance2);

	UART_IDLE_IRQ_ENABLE(&huart2);
#endif

#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	uart_instance3.instance    = UART_INSTANCE3;
	uart_instance3.fp_transmit = uart_instance3_transmit_start_cb;
	uart_instance3.fp_receive  = uart_instance3_receive_start_cb;
	uart_instance3.fp_event    = uart_instance_event_handle;
	uart_instance3_Init(&uart_instance3);

	UART_IDLE_IRQ_ENABLE(&huart3);
#endif

#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	uart_instance4.instance    = UART_INSTANCE4;
	uart_instance4.fp_transmit = uart_instance4_transmit_start_cb;
	uart_instance4.fp_receive  = uart_instance4_receive_start_cb;
	uart_instance4.fp_event    = uart_instance_event_handle;
	uart_instance4_Init(&uart_instance4);

	UART_IDLE_IRQ_ENABLE(&huart4);
#endif

#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
	uart_instance5.instance    = UART_INSTANCE5;
	uart_instance5.fp_transmit = uart_instance5_transmit_start_cb;
	uart_instance5.fp_receive  = uart_instance5_receive_start_cb;
	uart_instance5.fp_event    = uart_instance_event_handle;
	uart_instance5_Init(&uart_instance5);

	UART_IDLE_IRQ_ENABLE(&huart5);
#endif

#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	uart_instance6.instance    = UART_INSTANCE6;
	uart_instance6.fp_transmit = uart_instance6_transmit_start_cb;
	uart_instance6.fp_receive  = uart_instance6_receive_start_cb;
	uart_instance6.fp_event    = uart_instance_event_handle;
	uart_instance6_Init(&uart_instance6);

	UART_IDLE_IRQ_ENABLE(&huart6);
#endif

#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	uart_instance7.instance    = UART_INSTANCE7;
	uart_instance7.fp_transmit = uart_instance7_transmit_start_cb;
	uart_instance7.fp_receive  = uart_instance7_receive_start_cb;
	uart_instance7.fp_event    = uart_instance_event_handle;
	uart_instance7_Init(&uart_instance7);

	UART_IDLE_IRQ_ENABLE(&huart7);
#endif

#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	uart_instance8.instance    = UART_INSTANCE8;
	uart_instance8.fp_transmit = uart_instance8_transmit_start_cb;
	uart_instance8.fp_receive  = uart_instance8_receive_start_cb;
	uart_instance8.fp_event    = uart_instance_event_handle;
	uart_instance8_Init(&uart_instance8);

	UART_IDLE_IRQ_ENABLE(&huart8);
#endif

}

#if (0)
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	uart_error_irq_process(huart, APP_UART_IRQ_ERROR);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_tx_irq_process(huart, APP_UART_IRQ_TX);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_rx_irq_process(huart, APP_UART_IRQ_RX);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	uart_rx_irq_process(huart, APP_UART_IRQ_IDLE);
}

void USART_IDLE_line_IRQHandler(UART_HandleTypeDef *huart)
{
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
	{
		/* Clear IDLE line flag */
		__HAL_UART_CLEAR_IDLEFLAG(huart);

		uart_rx_irq_process(huart, APP_UART_IRQ_IDLE);
	}
}
#endif

/* Event callback app_uart_fifo */
static void uart_instance_event_handle(struct app_uart_fifo_ctx *p_app_cxt, app_uart_evt_t *p_event)
{
	switch (p_event->evt_type)
	{
	case EVT_APP_UART_TX_READY:
		break;

	case EVT_APP_UART_RX_READY:
		break;

	case EVT_APP_UART_TX_EMPTY:
		break;

	case EVT_APP_UART_RX_EMPTY:
		break;

	case EVT_APP_UART_TX_OVER:
		break;

	case EVT_APP_UART_RX_OVER:
		DBG_PRINTF("\r\n[UART_INSTANCE%u] EVT_APP_UART_RX_OVER", p_app_cxt->instance + 1);
		break;

	case EVT_APP_UART_DATA_READY:
		if (uart_error[p_app_cxt->instance] > 0)
		{
			uart_error[p_app_cxt->instance] = 0;
			DBG_PRINTF("\r\n[UART_INSTANCE%u] EVT_APP_UART_DATA_READY", p_app_cxt->instance + 1);
		}
		break;

	case EVT_APP_UART_DATA_SENDING:
		break;

	case EVT_APP_UART_TX_FIFO_ERROR:
		break;

	case EVT_APP_UART_RX_FIFO_ERROR:
		DBG_PRINTF("\r\n[UART_INSTANCE%u] EVT_APP_UART_RX_FIFO_ERROR", p_app_cxt->instance + 1);
		break;

	case EVT_APP_UART_COMMUNICATION_ERROR:
		if (p_app_cxt->instance < UART_INSTANCE_NUM)
		{
			uart_error[p_app_cxt->instance] = UART_ERROR_COUNT_DOWN;
			DBG_PRINTF("\r\n[UART_INSTANCE%u] EVT_APP_UART_COMMUNICATION_ERROR", p_app_cxt->instance + 1);
		}
		break;

	default:
		break;
	}
}
