#include "battery_spy.h"
#include "led.h"
#include "usart.h"
#include "adc_data.h"

#define HighState  1
#define LowState   0
#define NormalValue 2265 //3.65V == Low Power


/*********Bat_filter*******/
#define N 10 
uint16_t BatValueBuf[N];
char index_B=0;
uint16_t  sum=0;

/* 3.65/2/3.3*4096 = 2265 */
uint16_t Up_edge = NormalValue+15;
uint16_t Down_edge = NormalValue-15;
uint16_t LI_Old_State=LowState;



void Bat_Spy()
{

	uint16_t LI_Capacity=sum;

	if((LI_Capacity>=Up_edge)||((LI_Capacity>Down_edge)&&(LI_Old_State==HighState)))
	{
		LI_Old_State = HighState;
    LED_blue();
	}
	if((LI_Capacity<=Down_edge)||((LI_Capacity<Up_edge)&&(LI_Old_State==LowState)))
	{
		LI_Old_State = LowState;
		LED_red();
	}

}


void Bat_Filter(void)
{
	 sum=0;
   BatValueBuf[index_B++] = (uint16_t)Li_battery;
   if ( index_B == N )  
	 {
		 index_B = 0;
     for(char count=0;count<N;count++)
		 {
       sum += BatValueBuf[count];
		 }
		 sum/=N;
		 Bat_Spy();

	 }

}





