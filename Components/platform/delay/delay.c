/**
  ******************************************************************************
  * @file    delay.c
  * @author
  * @version V1.1.0
  * @date    11-march-2019
  * @brief   device mounted on STM32H743I boards.
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "delay.h"

/**
 * @brief  Initializes delay functions 
 * @param  None
 * @retval DWT counter start status
 *           - 0: DWT counter did not start, delay for microseconds won't work
 *           - > 0: DWT counter works OK, delay is ready to use
 */
uint32_t DELAY_Init(void)
{
#if !defined(STM32F0xx)
	uint32_t c;
	
    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;
	
    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;
	
    /* Reset counter */
    DWT->CYCCNT = 0;
	
    /* Check if DWT has started */
    c = DWT->CYCCNT;
    
    /* 2 dummys */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    
    /* Return difference, if result is zero, DWT has not started */
    return (DWT->CYCCNT - c);
#else
	/* Return OK */
	return 1;
#endif
}

__weak void delay(uint32_t ms)
{
  HAL_Delay(ms);
}

void delay_ms(uint32_t nms)
{
  HAL_Delay(nms);
}
