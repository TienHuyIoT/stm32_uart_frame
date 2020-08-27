#include "serial0.h"
#include "stm32h7xx_hal.h"
#include "app_uart_fifo.h"

app_uart_fifo_ctx_t	uart_instance0;
extern UART_HandleTypeDef huart3;

#define UART0_TX_BUF_SIZE 256   /**< UART TX buffer size is a power of two */
#define UART0_RX_BUF_SIZE 256   /**< UART RX buffer size is a power of two */

/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle.
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_instance0.irq_callback(&uart_instance0, APP_UART_IRQ_TX);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_instance0.irq_callback(&uart_instance0, APP_UART_IRQ_RX);
}

/* Event callback app_uart_fifo */
static void uart_instance0_event_handle(struct app_uart_fifo_ctx *p_app_cxt, app_uart_evt_t *p_event)
{
	if (p_app_cxt == &uart_instance0)
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
			break;

		case EVT_APP_UART_DATA_READY:
			break;

		case EVT_APP_UART_DATA_SENDING:
			break;

		case EVT_APP_UART_TX_FIFO_ERROR:
			break;

		case EVT_APP_UART_RX_FIFO_ERROR:
			break;

		case EVT_APP_UART_COMMUNICATION_ERROR:
			break;

		default:
			break;
		}
	}
}

static void uart_instance0_transmit_start(uint8_t *p_data, uint32_t lenght)
{
	HAL_UART_Transmit_DMA(&huart3, p_data, lenght);
}

static void uart_instance0_receive_start(uint8_t *p_data, uint32_t lenght)
{
	HAL_UART_Receive_IT(&huart3, p_data, lenght);
}

UART_Status_t uart_instance0_Init(void)
{
	uint32_t err_code;
	uart_instance0.fp_transmit = uart_instance0_transmit_start;
	uart_instance0.fp_receive = uart_instance0_receive_start;
	uart_instance0.fp_event = uart_instance0_event_handle;

	APP_UART_FIFO_INIT(&uart_instance0,
                         UART0_RX_BUF_SIZE,
                         UART0_TX_BUF_SIZE,
						 err_code);
    return (err_code == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

UART_Status_t uart_instance0_deinit(void)
{
  return UART_OK;
}

UART_Status_t putchar_instance0(char c)
{
	return (app_uart_put(&uart_instance0, (uint8_t)c) == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

void puts_uart_instance0(char *s)
{
  while(*s)
  {
    app_uart_put(&uart_instance0, (uint8_t)*s++);
  }
}

uint32_t write_uart_instance0(uint8_t *s, uint32_t len)
{
  uint32_t i;
  for(i = 0; i < len; i++)
  {
    if(app_uart_put(&uart_instance0, (uint8_t)(*s)) != APP_UART_FIFO_OK)
    {
      break;
    }
    s++;
  }
  return i;
}

/**
  * @brief get one character from Rx buff usart0
  * @param  none
  * @retval character
  */
UART_Status_t getchar_instance0(char *c)
{
    return (app_uart_get(&uart_instance0, (uint8_t *)c) == APP_UART_FIFO_OK)? UART_OK : UART_ERROR;
}

/**
  * @brief check rx buff usart0
  * @param  none
  * @retval size real buff available
  */
uint32_t uart_instance0_available(void){
  return app_uartRx_is_available(&uart_instance0);
}

uint32_t uart_instance0_can_send(void)
{
  return app_uartTx_can_send(&uart_instance0);
}

void uart_instance0_flush(void)
{
  app_uart_flush(&uart_instance0);
}

/* Test uart echo */
void uart_test_echo(void)
{
	if (uart_instance0_available())
	{
		char c;
		if (UART_OK == getchar_instance0(&c))
		{
			putchar_instance0(c);
		}
	}
}

