/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"

#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "app_user.h"
#include "led.h"
#include "motor.h"
#include "exit_work.h"
#include "battery_spy.h"
#include "adc_data.h"
#include "mpu6050.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Motor_Pin GPIO_PIN_13
#define Motor_GPIO_Port GPIOC
#define PWR_EN_Pin GPIO_PIN_14
#define PWR_EN_GPIO_Port GPIOC
#define EXIT_WKUP_Pin GPIO_PIN_0
#define EXIT_WKUP_GPIO_Port GPIOA
#define EXIT_WKUP_EXTI_IRQn EXTI0_IRQn
#define EXIT_MPU_Pin GPIO_PIN_2
#define EXIT_MPU_GPIO_Port GPIOB
#define EXIT_MPU_EXTI_IRQn EXTI2_IRQn
#define BOOTH_STATE_Pin GPIO_PIN_12
#define BOOTH_STATE_GPIO_Port GPIOB
#define BOOTH_LED_Pin GPIO_PIN_13
#define BOOTH_LED_GPIO_Port GPIOB
#define CHRG_Pin GPIO_PIN_12
#define CHRG_GPIO_Port GPIOA
#define BAT_GND_Pin GPIO_PIN_15
#define BAT_GND_GPIO_Port GPIOA
#define EXIT_WKUP2_Pin GPIO_PIN_4
#define EXIT_WKUP2_GPIO_Port GPIOB
#define EXIT_WKUP2_EXTI_IRQn EXTI4_IRQn
#define Power_ON_Pin GPIO_PIN_5
#define Power_ON_GPIO_Port GPIOB
#define RGB_G_Pin GPIO_PIN_6
#define RGB_G_GPIO_Port GPIOB
#define RGB_B_Pin GPIO_PIN_7
#define RGB_B_GPIO_Port GPIOB
#define RGB_R_Pin GPIO_PIN_8
#define RGB_R_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
