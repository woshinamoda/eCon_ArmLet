#include "led.h"



void LED_red(void)
{
		LED1_OFF();
		LED3_OFF();
		LED2_ON();
}
void LED_blue(void)
{
		LED2_OFF();
		LED1_OFF();
		LED3_ON();
}
void LED_green(void)
{
		LED3_OFF();
		LED2_OFF();
		LED1_ON();
}

void LED_white(void)
{
	LED1_ON();
	LED2_ON();
	LED3_ON();
}

void LED_close(void)
{
		LED1_OFF();
		LED3_OFF();
		LED2_OFF();
}


void LED_TOGGLE(enum col temp)
{
	if(temp==green)
	{
		LED1_TOG();
		LED2_OFF();
		LED3_OFF();
	}
	else if(temp==blue)
	{
		LED3_TOG();
		LED2_OFF();
		LED1_OFF();
	}
	else if(temp==red)
	{
		LED2_TOG();
		LED3_OFF();
		LED1_OFF();
	}
}



