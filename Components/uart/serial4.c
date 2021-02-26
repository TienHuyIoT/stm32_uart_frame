#include "serial4.h"

app_uart_fifo_ctx_t	*p_instance4;

#define UART4_TX_BUF_SIZE 256   /**< UART TX buffer size is a power of two */
#define UART4_RX_BUF_SIZE 256   /**< UART RX buffer size is a power of two */

UART_Status_t uart_instance4_Init(app_uart_fifo_ctx_t *p_uart_cxt)
{
	uint32_t err_code;
	p_instance4 = p_uart_cxt;

	APP_UART_FIFO_INIT(p_instance4,
                         UART4_RX_BUF_SIZE,
                         UART4_TX_BUF_SIZE,
						 err_code);
    return (err_code == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

UART_Status_t uart_instance4_deinit(void)
{
  return UART_OK;
}

int putchar_instance4(uint8_t c)
{
	return (app_uart_put(p_instance4, c) == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

void puts_uart_instance4(char *s)
{
  while(*s)
  {
    app_uart_put(p_instance4, (uint8_t)*s++);
  }
}

size_t write_uart_instance4(uint8_t *s, size_t len)
{
  app_uart_write(p_instance4, s, (uint32_t*)&len);

  return len;
}

/**
  * @brief get one character from Rx buff usart0
  * @param  none
  * @retval character
  */
int getchar_instance4(uint8_t *c)
{
    return (app_uart_get(p_instance4, c) == APP_UART_FIFO_OK)? UART_OK : UART_ERROR;
}

size_t read_uart_instance4(uint8_t *s, size_t len)
{
  size_t i;
  for(i = 0; i < len; i++)
  {
    if(app_uart_get(p_instance4, s) != APP_UART_FIFO_OK)
    {
      break;
    }
    s++;
  }
  return i;
}

void uart_instance4_rx_empty(void)
{
  uint8_t c;
  while(UART_OK == getchar_instance4(&c));
}

/**
  * @brief check rx buff usart0
  * @param  none
  * @retval size real buff available
  */
size_t uart_instance4_available(void)
{
  return app_uartRx_is_available(p_instance4);
}

size_t uart_instance4_can_send(void)
{
  return app_uartTx_can_send(p_instance4);
}

void uart_instance4_flush(void)
{
  app_uart_flush(p_instance4);
}

/* Test uart echo */
void uart_instance4_echo(void)
{
	if (uart_instance4_available())
	{
		uint8_t c;
		if (UART_OK == getchar_instance4(&c))
		{
			putchar_instance4(c);
		}
	}
}

