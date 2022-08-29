#include "bsp_usart.h"
#include "stm_delay.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/
/* ����2 TX = PD5   RX = PD6 ��  TX = PA2�� RX = PA3  */
/* ����3 TX = PB10   RX = PB11 */
/* ����4 TX = PC10   RX = PC11 */
/* ����5 TX = PC12   RX = PD2 */
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

	  /*����������ã����򴮿�1   ֻ�ܷ�������*/
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				 //ѡ�񴮿�1�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//��ռʽ�ж����ȼ�����Ϊ0
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //��Ӧʽ�ж����ȼ�����Ϊ0
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //ʹ���ж�
	  NVIC_Init(&NVIC_InitStructure);	  
}

/**
  * @��������:  ����USART�ܽš�ʱ�ӡ�������
  * @��������:  
		        COM{in}         : COM1��COM2   ��ʾUSART1 ��USART2
			    UsartBaudRate   : ����ͨ�Ų����� ��׼�����ٶ���       
  * @����ֵ  :  none
  * @˵  ��  :  �١�����ֻ������USART1��USART2������ӿ����������ַ�ʽ���
				�ڡ�����ͨ������Ϊ8bit 1stop û��Ӳ��������....
  * @ʹ�÷���:   bsp_InitUart(COM2,9600);  
                
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

   /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(COM_USART[COM], USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
}
/**
  * @��������:  �򴮿����ݼĴ���дһ��Byte
  * @��������:  
			    USARTx{in}      : ���ں� USART1 ��USART2��USART3��USART4��USART5
		     	byte            : Ҫд������       
  * @����ֵ  :  none
  * @ʹ�÷���:  USART_SedndByte(USART2,0xAA);                
  */
void USART_SendByte(USART_TypeDef* USARTx,uint8_t byte)
{ 
   	while (!(USARTx->SR & USART_FLAG_TXE));
   	USARTx->DR = (byte & (uint16_t)0x01FF);	   
}
/**
  * @��������:   �򴮿����ݼĴ���д������
  * @��������:  
				 USARTx{in}      : ���ں� USART1 ��USART2��USART3��USART4��USART5
				 data            : Ҫд�������ͷָ��(������)       
  * @����ֵ  :   none
  * @ʹ�÷���:   USART_SendStr(USART1,"I'M AUTO"); �����printf ����printf�ȶ�                
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
  * @��������:  �򴮿����ݼĴ���д��һ���ַ���
  * @��������:  
				USARTx{in}      : ���ں� USART1 ��USART2��USART3��USART4��USART5
				data            : Ҫд���ַ��� 
				len             : ����ĳ��� ����sizeof���  
  * @����ֵ  :  none
  * @˵  ��	 �� ����ֻ����unsigned char������  ��Ϊ���ﶨ�崮��ͨ����ByteΪ��λ
  * @ʹ�÷���:  u8 GPRS_login1[]={0xAA,0x02,0x00,0x14,0x01,0x02,0x01,0x00,0x0F,0x31,0x31,0x37,0x2E,0x30,0x33,0x34,
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
  * @��������:  ��ʾһ��16����
  * @��������:  
				USARTx{in}      : ���ں� USART1 ��USART2��USART3��USART4��USART5
				byte            : Ҫд������       
  * @����ֵ  :  none
  *	@ʹ�÷���:  USART_SedndByte(USART2,0xAA);                
  */
void USART_SendHex(USART_TypeDef* USARTx,uint8_t byte)
{ 
//����+30  ��д��ĸ+37  
		u8 temp_high;
		u8 temp_low;
		u8 temp=0x00;
		temp=byte; //��ò����ƻ� ʹ�ø���temp2
		temp_low=temp&0x0F;//������λ
	  //������λ
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
	USART_SendByte(USART1,0x0A); //�س�
}

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
unsigned char _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X0040)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/****************************************************************************************/

