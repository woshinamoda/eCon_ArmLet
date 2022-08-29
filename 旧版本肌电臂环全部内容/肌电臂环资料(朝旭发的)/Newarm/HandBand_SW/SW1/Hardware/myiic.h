#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32f4xx_hal.h"
#include "main.h"


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/* 定义读写SCL和SDA的宏 */
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式
//IO操作函数	 
#define IIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOB,IIC_SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,IIC_SCL_Pin,GPIO_PIN_RESET)) //SCL
#define IIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOB,IIC_SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,IIC_SDA_Pin,GPIO_PIN_RESET)) //SDA
#define READ_SDA   HAL_GPIO_ReadPin(GPIOB,IIC_SDA_Pin)  //输入SDA 
						 

//IIC所有操作函数
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);



#endif
















