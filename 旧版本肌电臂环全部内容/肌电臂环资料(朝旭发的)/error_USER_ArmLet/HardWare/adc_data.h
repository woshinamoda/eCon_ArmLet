#ifndef _ADC_DATA_h_
#define _ADC_DATA_h_

#include "stm32f4xx_hal.h"
#include "main.h"

extern uint32_t AD_Value[20];
extern uint32_t Li_battery;
extern uint8_t Emg_Data_Sent[20];      //Emg_Data_Sent



void ADCStart(void);
void get_ADCdata(void);


#endif 

//[] END OF FILE

