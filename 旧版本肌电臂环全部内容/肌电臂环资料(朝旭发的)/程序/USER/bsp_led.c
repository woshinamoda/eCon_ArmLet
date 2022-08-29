
#include "bsp_led.h"
#include "stm32f4xx.h"
#include "stm_delay.h"
#include "bsp_usart.h"

//LED端口定义
//LED3_OFF is blue
//LED2_OFF is green
//LED1_OFF is red

#define LED1_PIN                         GPIO_Pin_15
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOA   

#define LED2_PIN                         GPIO_Pin_3
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK                    RCC_AHB1Periph_GPIOB   

#define LED3_PIN                         GPIO_Pin_4
#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK                    RCC_AHB1Periph_GPIOB   


    
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED1_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	
	GPIO_Init(LED1_GPIO_PORT,&GPIO_InitStructure);
    
    RCC_AHB1PeriphClockCmd(LED2_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	
	GPIO_Init(LED2_GPIO_PORT,&GPIO_InitStructure);
    
    RCC_AHB1PeriphClockCmd(LED3_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	
	GPIO_Init(LED3_GPIO_PORT,&GPIO_InitStructure);
	
	//初始化检测电源的脚
	; //???  WK_UP-->GPIOA.0   
; //PA0?????,??     

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		LED1_ON();
    LED2_ON();
    LED3_OFF();
}
//LED3_OFF is blue
//LED2_OFF is green
//LED1_OFF is red

void LED_red(void)
{
		LED2_ON();
		LED3_ON();
		LED1_OFF();
}
void LED_blue(void)
{
		LED2_ON();
		LED1_ON();
		LED3_OFF();
}
void LED_green(void)
{
		LED1_ON();
		LED3_ON();
		LED2_OFF();
}
void LED_TOGGLE(enum col temp)
{
	if(temp==red)
	{
		LED1_TOG();
		LED2_ON();
		LED3_ON();
	}
	else if(temp==blue)
	{
		LED3_TOG();
		LED2_ON();
		LED1_ON();
	}
	else if(temp==green)
	{
		LED2_TOG();
		LED3_ON();
		LED1_ON();
	}
}

