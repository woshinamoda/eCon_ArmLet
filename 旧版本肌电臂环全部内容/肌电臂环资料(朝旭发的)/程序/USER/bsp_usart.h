#ifndef __BSP_USART_H
#define __BSP_USART_H

#include <stdio.h>
#include "stm32f4xx.h"


/* bmp Define ----------------------------------------------------------------*/
/* <1=> Enable, <0=> Disable */
#define _USE_DEBUG	      1 
#if _USE_DEBUG
	#define DbgPrintf(FORMAT, ...)      printf(FORMAT, ##__VA_ARGS__)
#else
	#define DbgPrintf(FORMAT, ...)     ;
#endif
/* bmp Define ----------------------------------------------------------------*/

/** @defgroup Exported_Types
  * @{
  */
#define COMn                             3	/*定义三个串口*/
typedef enum 
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2
} COM_TypeDef; 

/** @defgroup Exported_Functions
  * @{
  */

void USART_NVIC_cfg(void);
void bsp_InitUart(COM_TypeDef COM,u32 UsartBaudRate);
void USART1_Send_Byte(uint16_t Data);
void USART_SendByte(USART_TypeDef* USARTx,uint8_t byte);
void USART_SendStr(USART_TypeDef* USARTx,const char * data);
void USART_SendArray(USART_TypeDef* USARTx, u8 *data,u16 len);


#endif


