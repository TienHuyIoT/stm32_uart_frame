#include "serial_common.h"
#include "serial1.h"
#include "serial2.h"
#include "serial3.h"
#include "serial4.h"
#include "serial5.h"
#include "serial6.h"
#include "serial7.h"
#include "serial8.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

uint8_t putchar_instance;

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	switch (putchar_instance) {
#if (defined APP_UART_FIFO_1) && (APP_UART_FIFO_1 == 1)
	case UART_INSTANCE1:
		putchar_instance1(ch);
		break;
#endif
#if (defined APP_UART_FIFO_2) && (APP_UART_FIFO_2 == 1)
	case UART_INSTANCE2:
		putchar_instance2(ch);
		break;
#endif
#if (defined APP_UART_FIFO_3) && (APP_UART_FIFO_3 == 1)
	case UART_INSTANCE3:
		putchar_instance3(ch);
		break;
#endif
#if (defined APP_UART_FIFO_4) && (APP_UART_FIFO_4 == 1)
	case UART_INSTANCE4:
		putchar_instance4(ch);
		break;
#endif
#if (defined APP_UART_FIFO_5) && (APP_UART_FIFO_5 == 1)
	case UART_INSTANCE5:
		putchar_instance5(ch);
		break;
#endif
#if (defined APP_UART_FIFO_6) && (APP_UART_FIFO_6 == 1)
	case UART_INSTANCE6:
		putchar_instance6(ch);
		break;
#endif
#if (defined APP_UART_FIFO_7) && (APP_UART_FIFO_7 == 1)
	case UART_INSTANCE7:
		putchar_instance7(ch);
		break;
#endif
#if (defined APP_UART_FIFO_8) && (APP_UART_FIFO_8 == 1)
	case UART_INSTANCE8:
		putchar_instance8(ch);
		break;
#endif
}

	return ch;
}
