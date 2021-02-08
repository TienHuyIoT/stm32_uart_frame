#include "serial4.h"

app_uart_fifo_ctx_t	*p_instance5;

#define UART5_TX_BUF_SIZE 256   /**< UART TX buffer size is a power of two */
#define UART5_RX_BUF_SIZE 256   /**< UART RX buffer size is a power of two */

UART_Status_t uart_instance5_Init(app_uart_fifo_ctx_t *p_uart_cxt)
{
	uint32_t err_code;
	p_instance5 = p_uart_cxt;

	APP_UART_FIFO_INIT(p_instance5,
                         UART5_RX_BUF_SIZE,
                         UART5_TX_BUF_SIZE,
						 err_code);
    return (err_code == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

UART_Status_t uart_instance5_deinit(void)
{
  return UART_OK;
}

int putchar_instance5(uint8_t c)
{
	return (app_uart_put(p_instance5, c) == APP_UART_FIFO_OK) ? UART_OK : UART_ERROR;
}

void puts_uart_instance5(char *s)
{
  while(*s)
  {
    app_uart_put(p_instance5, (uint8_t)*s++);
  }
}

size_t write_uart_instance5(uint8_t *s, size_t len)
{
  size_t i;
  for(i = 0; i < len; i++)
  {
    if(app_uart_put(p_instance5, (uint8_t)(*s)) != APP_UART_FIFO_OK)
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
int getchar_instance5(uint8_t *c)
{
    return (app_uart_get(p_instance5, c) == APP_UART_FIFO_OK)? UART_OK : UART_ERROR;
}

size_t read_uart_instance5(uint8_t *s, size_t len)
{
  size_t i;
  for(i = 0; i < len; i++)
  {
    if(app_uart_get(p_instance5, s) != APP_UART_FIFO_OK)
    {
      break;
    }
    s++;
  }
  return i;
}

/**
  * @brief check rx buff usart0
  * @param  none
  * @retval size real buff available
  */
size_t uart_instance5_available(void)
{
  return app_uartRx_is_available(p_instance5);
}

size_t uart_instance5_can_send(void)
{
  return app_uartTx_can_send(p_instance5);
}

void uart_instance5_flush(void)
{
  app_uart_flush(p_instance5);
}

/* Test uart echo */
void uart_instance5_echo(void)
{
	if (uart_instance5_available())
	{
		uint8_t c;
		if (UART_OK == getchar_instance5(&c))
		{
			putchar_instance5(c);
		}
	}
}

