#include "serial8.h"

app_uart_fifo_ctx_t	*p_instance8;

#define UART8_TX_BUF_SIZE 256   /**< UART TX buffer size is a power of two */
#define UART8_RX_BUF_SIZE 256   /**< UART RX buffer size is a power of two */

UART_Status_t uart_instance8_Init(app_uart_fifo_ctx_t *p_uart_cxt)
{
	uint32_t err_code;
	p_instance8 = p_uart_cxt;

	APP_UART_FIFO_INIT(p_instance8,
                         UART8_RX_BUF_SIZE,
                         UART8_TX_BUF_SIZE,
						 err_code);
    return (err_code == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

UART_Status_t uart_instance8_deinit(void)
{
  return UART_OK;
}

int putchar_instance8(uint8_t c)
{
	return (app_uart_put(p_instance8, c) == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

void puts_uart_instance8(char *s)
{
  while(*s)
  {
    app_uart_put(p_instance8, (uint8_t)*s++);
  }
}

size_t write_uart_instance8(uint8_t *s, size_t len)
{
  app_uart_write(p_instance8, s, (uint32_t*)&len);

  return len;
}

/**
  * @brief get one character from Rx buff usart0
  * @param  none
  * @retval character
  */
int getchar_instance8(uint8_t *c)
{
    return (app_uart_get(p_instance8, c) == APP_UART_FIFO_OK)? UART_OK : UART_ERROR;
}

size_t read_uart_instance8(uint8_t *s, size_t len)
{
  size_t i;
  for(i = 0; i < len; i++)
  {
    if(app_uart_get(p_instance8, s) != APP_UART_FIFO_OK)
    {
      break;
    }
    s++;
  }
  return i;
}

void uart_instance8_rx_empty(void)
{
  uint8_t c;
  while(UART_OK == getchar_instance8(&c));
}

/**
  * @brief check rx buff usart0
  * @param  none
  * @retval size real buff available
  */
size_t uart_instance8_available(void)
{
  return app_uartRx_is_available(p_instance8);
}

size_t uart_instance8_can_send(void)
{
  return app_uartTx_can_send(p_instance8);
}

void uart_instance8_flush(void)
{
  app_uart_flush(p_instance8);
}

/* Test uart echo */
void uart_instance8_echo(void)
{
	if (uart_instance8_available())
	{
		uint8_t c;
		if (UART_OK == getchar_instance8(&c))
		{
			putchar_instance8(c);
		}
	}
}

