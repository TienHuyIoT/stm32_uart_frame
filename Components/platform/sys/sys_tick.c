/**
  ******************************************************************************
  * @file    sys_tick.c
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    11-march-2019
  * @brief   device mounted on STM32H743I-NEO boards.
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "sys_tick.h"

/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetSysClockFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
       Note: If you use this function to configure the system clock; then there
             is no need to call the 2 first functions listed above, since SystemCoreClock
             variable is updated automatically.
*/
extern uint32_t SystemCoreClock;             /*!< System Domain1 Clock Frequency  */

/* Overlap  after ~50 days */
uint32_t millis(void)
{
  return HAL_GetTick();
}

/* Overlap  after ~71 mins */
uint32_t micros(void)
{
  return (DWT->CYCCNT / (SystemCoreClock / 1000000) + (HAL_GetTick() * 1000));
}
