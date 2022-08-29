#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H
#include "stm32f4xx.h"
	 
#define Motor_ON()                        PBout(12) = 0
#define Motor_OFF()                       PBout(12) = 1
	 

void Motor_Init(void);//≥ı ºªØ	

void Motor_Long_Strong(void);
void Motor_Short_Strong(void);
void Motor_Short_Soft(void);
u8 MotorSetType(char inputType,u32 *timeMotor);
#endif