#include "bsp_usart.h"
#include "stm_delay.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* 串口1 TX = PA9   RX = PA10 或 TX = PB6   RX = PB7*/
/* 串口2 TX = PD5   RX = PD6 或  TX = PA2， RX = PA3  */
/* 串口3 TX = PB10   RX = PB11 */
/* 串口4 TX = PC10   RX = PC11 */
/* 串口5 TX = PC12   RX = PD2 */
/* PG14/USART6_TX , PC7/USART6_RX,PG8/USART6_RTS, PG15/USART6_CTS */
USART_TypeDef* COM_USART[COMn] = {USART1,USART2,USART3}; 
GPIO_TypeDef*  COM_TX_PORT[COMn] = {GPIOA,GPIOA,GPIOB}; 
GPIO_TypeDef*  COM_RX_PORT[COMn] = {GPIOA,GPIOA,GPIOB}; 
const uint32_t COM_USART_CLK[COMn] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3};
const uint32_t COM_TX_PORT_CLK[COMn] = {RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB};
const uint32_t COM_RX_PORT_CLK[COMn] = {RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB};
const uint16_t COM_TX_PIN[COMn] = {GPIO_Pin_9, GPIO_Pin_2,GPIO_Pin_10};
const uint16_t COM_RX_PIN[COMn] = {GPIO_Pin_10, GPIO_Pin_3,GPIO_Pin_11};
const uint16_t COM_TX_PIN_SOURCE[COMn] = {GPIO_PinSource9,GPIO_PinSource2,GPIO_PinSource10};
const uint16_t COM_RX_PIN_SOURCE[COMn] = {GPIO_PinSource10,GPIO_PinSource3,GPIO_PinSource11};
const uint16_t COM_TX_AF[COMn] = {GPIO_AF_USART1,GPIO_AF_USART2,GPIO_AF_USART3};
const uint16_t COM_RX_AF[COMn] = {GPIO_AF_USART1,GPIO_AF_USART2,GPIO_AF_USART3};

void USART_NVIC_cfg(void)
{
	  NVIC_InitTypeDef NVIC_InitStructure;		

	  /*下面必须配置，否则串口1   只能发不能收*/
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				 //选择串口1中断
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//抢占式中断优先级设置为0
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //响应式中断优先级设置为0
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //使能中断
	  NVIC_Init(&NVIC_InitStructure);	  
}

/**
  * @函数功能:  配置USART管脚、时钟、波特率
  * @函数参数:  
		        COM{in}         : COM1、COM2   表示USART1 、USART2
			    UsartBaudRate   : 串口通信波特率 标准串口速度有       
  * @返回值  :  none
  * @说  明  :  ①、这里只配置了USART1、USART2如需添加可以依照这种方式添加
				②、这里通用配置为8bit 1stop 没有硬件流控制....
  * @使用范例:   bsp_InitUart(COM2,9600);  
                
*/
void bsp_InitUart(COM_TypeDef COM,u32 UsartBaudRate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = UsartBaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

  /* Enable UART clock */
  if (COM == COM1)
  {
       RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  }
  else
  {
       RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_PIN_SOURCE[COM], COM_TX_AF[COM]);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_RX_AF[COM]);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], &USART_InitStructure);

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	  
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
  USART_NVIC_cfg();

   /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(COM_USART[COM], USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
}
/**
  * @函数功能:  向串口数据寄存器写一个Byte
  * @函数参数:  
			    USARTx{in}      : 串口号 USART1 、USART2、USART3、USART4、USART5
		     	byte            : 要写的数据       
  * @返回值  :  none
  * @使用范例:  USART_SedndByte(USART2,0xAA);                
  */
void USART_SendByte(USART_TypeDef* USARTx,uint8_t byte)
{ 
   	while (!(USARTx->SR & USART_FLAG_TXE));
   	USARTx->DR = (byte & (uint16_t)0x01FF);	   
}
/**
  * @函数功能:   向串口数据寄存器写入数组
  * @函数参数:  
				 USARTx{in}      : 串口号 USART1 、USART2、USART3、USART4、USART5
				 data            : 要写的数组的头指针(数组名)       
  * @返回值  :   none
  * @使用范例:   USART_SendStr(USART1,"I'M AUTO"); 相对于printf 但比printf稳定                
  */
void USART_SendStr(USART_TypeDef* USARTx,const char * data)
{
		u16 i;
		u16 len = strlen(data);
		for (i=0; i<len; i++)
		{
			USART_SendByte(USARTx,data[i]);
		}
}
/**
  * @函数功能:  向串口数据寄存器写入一串字符串
  * @函数参数:  
				USARTx{in}      : 串口号 USART1 、USART2、USART3、USART4、USART5
				data            : 要写的字符串 
				len             : 数组的长度 可用sizeof求得  
  * @返回值  :  none
  * @说  明	 ： 这里只做了unsigned char的数组  因为这里定义串口通信以Byte为单位
  * @使用范例:  u8 GPRS_login1[]={0xAA,0x02,0x00,0x14,0x01,0x02,0x01,0x00,0x0F,0x31,0x31,0x37,0x2E,0x30,0x33,0x34,
                               0x2E,0x30,0x39,0x30,0x2E,0x31,0x31,0x36,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F};
								 USART_SendArray(USART1,GPRS_login1,sizeof(GPRS_login1));                 
  */
void USART_SendArray(USART_TypeDef* USARTx, u8 *data,u16 len)
{
	u16 i;
	for (i=0; i<len; i++)
	{
		USART_SendByte(USARTx,data[i]);
	}
}
/**
  * @函数功能:  显示一个16进制
  * @函数参数:  
				USARTx{in}      : 串口号 USART1 、USART2、USART3、USART4、USART5
				byte            : 要写的数据       
  * @返回值  :  none
  *	@使用范例:  USART_SedndByte(USART2,0xAA);                
  */
void USART_SendHex(USART_TypeDef* USARTx,uint8_t byte)
{ 
//数字+30  大写字母+37  
		u8 temp_high;
		u8 temp_low;
		u8 temp=0x00;
		temp=byte; //存好不被破坏 使用副本temp2
		temp_low=temp&0x0F;//保留低位
	  //保留低位
		if(temp_low<0x0A)
			{
			 	temp_low=temp_low+0x30;
			}
		else
		{
			 temp_low=temp_low+0x37;
		}
		temp=byte;
	    temp_high=(temp>>4)&0x0F;
		if(temp_high<0x0A)
			{
			 	temp_high=temp_high+0x30;
			}
		else
		{
			 temp_high=temp_high+0x37;
		}
		USART_SendByte(USARTx,temp_high);
		USART_SendByte(USARTx,temp_low);
}
void printEnter(void)
{
	USART_SendByte(USART1,0x0D); //
	USART_SendByte(USART1,0x0A); //回车
}

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
unsigned char _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X0040)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/****************************************************************************************/

