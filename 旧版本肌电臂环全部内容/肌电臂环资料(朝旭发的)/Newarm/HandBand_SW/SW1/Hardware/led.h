#ifndef _LED_h_
#define _LED_h_

#include "stm32f4xx_hal.h"
#include "main.h"

enum col{green,red,blue};
#define LED1_ON()        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
#define LED1_OFF()       HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#define LED1_TOG()		   HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

#define LED2_ON()        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
#define LED2_OFF()       HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
#define LED2_TOG()		   HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

#define LED3_ON()        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
#define LED3_OFF()       HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
#define LED3_TOG()		   HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);


void LED_red(void);
void LED_blue(void);
void LED_green(void);
void LED_white(void);
void LED_close(void);
void LED_TOGGLE(enum col temp);





#endif 

//[] END OF FILE

