#ifndef _ADC_DATA_h_
#define _ADC_DATA_h_

#include "stm32f4xx_hal.h"
#include "main.h"

extern uint32_t AD_Value[20];
extern uint32_t Li_battery;
extern uint8_t 	Emg_Data_Sent[20];        //Emg_Data_Sent
extern uint8_t	Emg_With_IMU_sent[30];


void ADCStart(void);
void get_ADCdata(void);
uint8_t Get_Crc8(uint8_t *ptr,uint16_t len); 
uint8_t crcCode(uint8_t *ptr,uint16_t len);
void get_ADCdata_Double_for_Onechannel(void);
void Get_ADCdata_With_IMU(void);
#endif 

//[] END OF FILE

