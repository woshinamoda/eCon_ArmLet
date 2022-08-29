/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))

#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))

#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))



//IO鍙ｅ湴鍧?鏄犲�?

#define GPIOA_ODR_Addr (GPIOA_BASE+20) //0x40020014

#define GPIOB_ODR_Addr (GPIOB_BASE+20) //0x40020414

#define GPIOC_ODR_Addr (GPIOC_BASE+20) //0x40020814

#define GPIOD_ODR_Addr (GPIOD_BASE+20) //0x40020C14

#define GPIOE_ODR_Addr (GPIOE_BASE+20) //0x40021014

#define GPIOF_ODR_Addr (GPIOF_BASE+20) //0x40021414

#define GPIOG_ODR_Addr (GPIOG_BASE+20) //0x40021814

#define GPIOH_ODR_Addr (GPIOH_BASE+20) //0x40021C14

#define GPIOI_ODR_Addr (GPIOI_BASE+20) //0x40022014



#define GPIOA_IDR_Addr (GPIOA_BASE+16) //0x40020010

#define GPIOB_IDR_Addr (GPIOB_BASE+16) //0x40020410

#define GPIOC_IDR_Addr (GPIOC_BASE+16) //0x40020810

#define GPIOD_IDR_Addr (GPIOD_BASE+16) //0x40020C10

#define GPIOE_IDR_Addr (GPIOE_BASE+16) //0x40021010

#define GPIOF_IDR_Addr (GPIOF_BASE+16) //0x40021410

#define GPIOG_IDR_Addr (GPIOG_BASE+16) //0x40021810

#define GPIOH_IDR_Addr (GPIOH_BASE+16) //0x40021C10

#define GPIOI_IDR_Addr (GPIOI_BASE+16) //0x40022010



//IO鍙ｆ搷浣?

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr,n) //杈撳�?

#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr,n) //杈撳�?



#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr,n) //杈撳�?

#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr,n) //杈撳�?



#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr,n) //杈撳�?

#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr,n) //杈撳�?



#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr,n) //杈撳�?

#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr,n) //杈撳�?



#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr,n) //杈撳�?

#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr,n) //杈撳�?



#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr,n) //杈撳�?

#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr,n) //杈撳�?



#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr,n) //杈撳�?

#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr,n) //杈撳�?



#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr,n) //杈撳�?

#define PHin(n) BIT_ADDR(GPIOH_IDR_Addr,n) //杈撳�?



#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr,n) //杈撳�?

#define PIin(n) BIT_ADDR(GPIOI_IDR_Addr,n) //杈撳�?
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
