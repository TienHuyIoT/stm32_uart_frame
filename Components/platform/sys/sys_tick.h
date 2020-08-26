/**
  ******************************************************************************
  * @file    sys_tick.h
  * @author  HuyHT1-VF Team
  * @version V1.1.0
  * @date    1-March-2019
  * @brief   This file contains the common defines and functions prototypes for
  *          the sys_tick.c driver.
  ******************************************************************************
  */
#ifndef __SYS_TICK_H
#define __SYS_TICK_H

#ifdef __cplusplus
 extern "C" {
#endif
   
#include <stdint.h>   

uint32_t millis(void);
uint32_t micros(void); 
   
#ifdef __cplusplus
}
#endif

#endif  /* __SYS_TICK_H */