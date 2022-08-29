
#include "bsp_motor.h"
#include "stm32f4xx.h"
#include "stm_sys.h"
#include "stm_delay.h"
#include "bsp_usart.h"

//Motor端口定义
#define Motor_PIN                         GPIO_Pin_12
#define Motor_GPIO_PORT                   GPIOB
#define Motor_GPIO_CLK                    RCC_AHB1Periph_GPIOB   
#define MotorLong 150
#define MotorShort 50
#define MotorInterval 50

u8 k;

//Motor IO初始化
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(Motor_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=Motor_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	
	GPIO_Init(Motor_GPIO_PORT,&GPIO_InitStructure);
	
	Motor_OFF();
}

void Motor_Long_Strong(void)
{
	Motor_ON();
	delay_ms(2000);
	Motor_OFF();
}

void Motor_Short_Strong(void)
{
	Motor_ON();
	delay_ms(500);
	Motor_OFF();
}

void Motor_Short_Soft(void)
{
    for(k=0;k<20;k++)
    {
	   Motor_ON();
	   delay_ms(10);
	   Motor_OFF();
	   delay_ms(25);
    }
		delay_ms(500);
}

u8 MotorSetType(char inputType,u32 *timeMotor)
{
	u8 lengthReturn=4;
	switch(inputType)
	{
		case 0:
			timeMotor[0]=MotorLong;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorLong;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorLong;
			break;
		case 1:
			timeMotor[0]=MotorShort;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorShort;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorShort;
			break;
		case 2:
			timeMotor[0]=MotorShort;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorLong;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorLong;
			break;
		case 3:
			timeMotor[0]=MotorShort;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorShort;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorLong;
			break;
		case 4:
			timeMotor[0]=MotorLong;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorShort;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorLong;
			break;
		case 5:
			timeMotor[0]=MotorLong;timeMotor[1]=MotorInterval;
			timeMotor[2]=MotorLong;timeMotor[3]=MotorInterval;
			timeMotor[4]=MotorShort;
			break;
		default:
			break;
	}
	return lengthReturn;
}


