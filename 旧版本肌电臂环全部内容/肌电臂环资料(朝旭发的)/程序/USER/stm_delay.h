#ifndef __STM_DELAY_H
#define __STM_DELAY_H 			   
#include "stm32f4xx.h"


/** @defgroup Exported_Functions
  * @{
  */
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
void delay_Second(u8 second);
void get_ms(unsigned long *time);

#endif





























