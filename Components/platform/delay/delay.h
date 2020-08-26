/**
  ******************************************************************************
  * @file    delay.h
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    1-March-2019
  * @brief   This file contains the common defines and functions prototypes for
  *          the delay.c driver.
  ******************************************************************************
  */
#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif
      
#include "stm32h7xx_hal.h"
   
/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetSysClockFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
       Note: If you use this function to configure the system clock; then there
             is no need to call the 2 first functions listed above, since SystemCoreClock
             variable is updated automatically.
*/
extern uint32_t SystemCoreClock;             /*!< System Domain1 Clock Frequency  */   
   
uint32_t DELAY_Init(void);

/**
 * @brief  Delays for amount of micro seconds
 * @param  micros: Number of microseconds for delay
 * @retval None
 */
__STATIC_INLINE void delay_us(uint32_t micros) {
#if !defined(STM32F0xx)
	uint32_t start = DWT->CYCCNT;
	
	/* Go to number of cycles for system */
	micros *= (SystemCoreClock / 1000000);
	
	/* Delay till end */
	while ((DWT->CYCCNT - start) < micros);
#else
	/* Go to clock cycles */
	micros *= (SystemCoreClock / 1000000) / 5;
	
	/* Wait till done */
	while (micros--);
#endif
}
   
void delay(uint32_t ms);
void delay_ms(uint32_t nms);
   
#ifdef __cplusplus
}
#endif

#endif  /* __DELAY_H */
