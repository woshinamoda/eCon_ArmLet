#include "adc_data.h"
#include "adc.h"

uint32_t AD_Value[9]={0};/* 8->EMG;1->battery*/
uint32_t AD_Value_Temp[9]={0};
uint8_t Emg_Data_Sent[20]={0x55,0xAA};      //Emg_Data_Sent
uint8_t AD_Sent[20]={0};

uint32_t Li_battery=0;


void ADCStart(void)
{
	HAL_ADC_Start_DMA(&hadc1,AD_Value,9);//ADC循环模式开启
}


void get_ADCdata(void)
{
	uint8_t temp;
	/* 先读取，防止被刷新*/
	for(int j=0;j<8;j++)
	{
			AD_Value_Temp[j]=AD_Value[j];
	}
	/* read battery value */
	Li_battery=AD_Value[8];
	/* 1.5字节（12位）x8 = 12字节*/
	for (int n=0;n<4;n++)
	{
	   AD_Sent[3*n]=AD_Value_Temp[2*n]>>4;
		
	   AD_Sent[3*n+1]=(AD_Value_Temp[2*n]&0x000f);
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]<<4;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]&0xf0;
		 temp=(AD_Value_Temp[2*n+1]&0x0f00)>>8;
		 temp=temp&0x0f;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]|temp;
		 AD_Sent[3*n+2]=AD_Value_Temp[2*n+1]&0x00ff; 
	}
  /* 拼接，主程序直接发送*/
	Emg_Data_Sent[14] = 0;/*check sum empty*/
	Emg_Data_Sent[15] = 0;/*Trigger empty*/
	for(int i=2;i<14;i++)
	{
			Emg_Data_Sent[i]=AD_Sent[i-2];
		  Emg_Data_Sent[14] += Emg_Data_Sent[i];/*check sum*/
	}

		
	
}


