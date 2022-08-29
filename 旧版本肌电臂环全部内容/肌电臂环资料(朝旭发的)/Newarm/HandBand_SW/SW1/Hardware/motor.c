#include "motor.h"

#define Motor_ON()             HAL_GPIO_WritePin(GPIOB, MOTOR_CTRL_Pin, GPIO_PIN_RESET);
#define Motor_OFF()            HAL_GPIO_WritePin(GPIOB, MOTOR_CTRL_Pin, GPIO_PIN_SET);


void Motor_Long_Strong(void)
{
	Motor_ON();
	HAL_Delay(2000);
	Motor_OFF();
}

void Motor_Short_Strong(void)
{
	Motor_ON();
	HAL_Delay(500);
	Motor_OFF();
}

void Motor_Short_Soft(void)
{
    for(uint8_t k=0;k<20;k++)
    {
	   Motor_ON();
	   HAL_Delay(10);
	   Motor_OFF();
	   HAL_Delay(25);
    }
		HAL_Delay(500);
}

