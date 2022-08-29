
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_conf.h"
#include "stm_ADC.h"
#include "bsp_usart.h"
#include "mpu9150.h"

#define ADC1_DR_Address  ((uint32_t)0X4001204C)

static __IO uint16_t AD_Value[9]={0};
static __IO uint16_t AD_Volt[9]={0};
u8 AD_Sent[64]={0};

static __IO uint8_t m=0,n=0,h=0;
static u16 i=0;
static u32 sum=0;
u8 mpudata[18]={0};
//static u16 ind_mpu=0;

u8 Emg_Data_Sent[20]={0x55,0xAA};      //Emg_Data_Sent
u8 Imu_Data_Sent[30]={0x66,0xBB};      //Imu_Data_Sent
u8 if_imu_sent_flag = 0;

NVIC_InitTypeDef      NVIC_InitStructure;
GPIO_InitTypeDef      GPIO_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
ADC_InitTypeDef       ADC_InitStructure;
DMA_InitTypeDef       DMA_InitStructure;
DMA_Stream_TypeDef    DMA_Streamx;

void RCC_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_DMA2,ENABLE);
}
void DMA_Config(void)
{
	while (DMA_GetCmdStatus(&DMA_Streamx) != DISABLE){}//等待 DMA 可配置

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)ADC1_DR_Address;  //ADC1 DATA ADDRESS
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&AD_Value;    //DMA memory base address
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//
    DMA_InitStructure.DMA_BufferSize =9; //
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);
}

void Gpio_Config(void)
{
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ADC_Config(void)
{
  //ADC common
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	//zhaobin 1128
	//ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; 
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8; 
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  //ADC1
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_NbrOfConversion = 9;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_Init(ADC1,&ADC_InitStructure);
  
  //ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_15Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_3,3,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_4,4,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_5,5,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_6,6,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_7,7,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_8,8,ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_9,9,ADC_SampleTime_144Cycles);

  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);//使能ADC1_DMA
  ADC_Cmd(ADC1, ENABLE);  
}

void init_ADC(void)
{
	RCC_Config();
	Gpio_Config();
	ADC_Config();
	DMA_Config();
	
  ADC_SoftwareStartConv(ADC1);
}

void filter(void)
{
	/*
   for (n=0;n<9;n++)
   {
      for(i=0;i<100;i++) sum+=AD_Value[n];
      AD_Value[n]=sum/100;
      sum = 0;
   } 
	*/
}

//CRC8校验 
//ptr:要校验的数组 
//len:数组长度 
//返回值:CRC8码 
u8 Get_Crc8(u8 *ptr,u16 len) 
{ 
    u8 crc; 
    u8 i; 
    crc=0; 
    while(len--) 
    { 
        crc^=*ptr++; 
        for(i=0;i<8;i++) 
        { 
            if(crc&0x01)crc=(crc>>1)^0x8C; 
            else crc >>= 1; 
        } 
    } 
    return crc; 
} 
u8 crcCode(u8 *ptr,u16 len)
{
	char crc=0;
	char crcCode;
	for(int i=2;i<len;i++)
	{
		crc+=ptr[i];
	}
	crcCode=crc%256;
	return crcCode;
}
void encryptCode(u8 *ptr,u16 len) 
{
	u8 tempFir=0;
	u8 tempSec=0;
	for(n=0;n<len;n++)
	{
		tempFir=ptr[n]<<4;
		tempSec=ptr[n]>>4;
		ptr[n]=tempFir|tempSec;
		tempFir=~ptr[n]&0x55;
		tempSec=ptr[n]&0xAA;
		ptr[n]=tempFir|tempSec;
	}
}
u8 deencyptCode(u8 input)
{
	u8 tempFir=0;
	u8 tempSec=0;

	tempFir=input<<4;
	tempSec=input>>4;
	input=tempFir|tempSec;
	tempFir=~input&0x55;
	tempSec=input&0xAA;
	input=tempFir|tempSec;
	return input;
}
void get_ADCdata(void)
{
	u8 temp;
	Li_battery=AD_Value[8];
	//Li_battery=1000;
	/*for (n=0;n<8;n++)
	{
	   AD_Sent[2*n]=AD_Value[n]>>8;
	   AD_Sent[2*n+1]=AD_Value[n]&0x00ff; 
	}*/
	for (n=0;n<4;n++)
	{
	   AD_Sent[3*n]=AD_Value[2*n]>>4;
		
	   AD_Sent[3*n+1]=(AD_Value[2*n]&0x000f);
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]<<4;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]&0xf0;
		 temp=(AD_Value[2*n+1]&0x0f00)>>8;
		 temp=temp&0x0f;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]|temp;
		 AD_Sent[3*n+2]=AD_Value[2*n+1]&0x00ff; 
	}
	//zhaobin 1212
	Emg_Data_Sent[0]=0x55;
	Emg_Data_Sent[1]=0xAA;
	for(i=2;i<14;i++)
	{
			Emg_Data_Sent[i]=AD_Sent[i-2];
	}
	//Emg_Data_Sent[18]= Get_Crc8(Emg_Data_Sent,18);
	Emg_Data_Sent[14]=crcCode(Emg_Data_Sent,14);

	encryptCode(Emg_Data_Sent, 15) ;
	USART_SendArray(USART1,Emg_Data_Sent,15);
}

//zhaobin 1212 send the mpu data
void send_mpu()
{
	Imu_Data_Sent[0]=0x66;
	Imu_Data_Sent[1]=0xBB;
	
	if(get_mpu9150_data(mpudata)==1)
	{
		for(i=2;i<=19;i++)
		{
			//Imu_Data_Sent[i]=MPU_Sent[i-2];
			Imu_Data_Sent[i]=mpudata[i-2];
		}
		Imu_Data_Sent[20]= crcCode(Imu_Data_Sent,20);
		encryptCode(Imu_Data_Sent, 21) ;
		if_imu_sent_flag = 1;
//		USART_SendArray(USART1,Imu_Data_Sent,21);
	}
}
