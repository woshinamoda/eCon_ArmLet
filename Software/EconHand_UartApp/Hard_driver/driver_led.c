#include "driver_led.h"

/* private User code set**************************************************/
/* User code begin */




/* User code stop */


/* private function LED drive*********************************************/
/* User code begin */
void EconHand_BSP_LED_gpio_init()
{
	nrf_gpio_cfg_output(LED_R);
	nrf_gpio_cfg_output(LED_G);
	nrf_gpio_cfg_output(LED_B);

	//对应关闭所有LED灯
	led_blue_off;
	led_green_off;
	led_red_off;
}


void LED_Reset_close()
{//LED全部熄灭复位
	led_blue_off;
	led_green_off;
	led_red_off;
}

void LED_Blue_Fast_Blink(void)
{//蓝色LED快速闪烁		550ms/tick
	RGB_UserDef.RGB_count++;
	switch(RGB_UserDef.RGB_count)
	{
		case 40:	led_blue_on; 		break;
		case 50: 	led_blue_off;		break;
	}
	if(RGB_UserDef.RGB_count > 55)
	{
		led_blue_off;
		RGB_UserDef.RGB_count = 0;
	}
}

void LED_Blue_Slow_Blink(void)
{//蓝色LED慢速闪烁	
	RGB_UserDef.RGB_count++;
	switch(RGB_UserDef.RGB_count)
	{
		case 20:	led_blue_on; 		break;
		case 40: 	led_blue_off;		break;
	}
	if(RGB_UserDef.RGB_count > 300)
	{
		led_blue_off;
		RGB_UserDef.RGB_count = 0;
	}
}

void LED_Red_Slow_Blink()
{//红色LED慢速闪烁
	RGB_UserDef.RGB_count++;
	switch(RGB_UserDef.RGB_count)
	{
		case 20:	led_red_on; 		break;
		case 40: 	led_red_off;		break;
	}
	if(RGB_UserDef.RGB_count > 300)
	{
		led_red_off;
		RGB_UserDef.RGB_count = 0;
	}
}


void LED_Blue_Open(void)
{//蓝色LED常亮
	led_blue_on;
}

void LED_Green_Open(void)
{//绿色LED常亮
	led_green_on;
}


void	APP_Order_Blink()
{//收到APP下发指令，红蓝交替闪烁50ms 2次
	RGB_UserDef.RGB_count++;
	switch(RGB_UserDef.RGB_count)
	{
		case 1:	led_red_on;			led_blue_off;		break;
		case 5:	led_red_off;		led_blue_on;		break;
			
		case 10:led_red_on;			led_blue_off;		break;
		case 15:led_red_off;		led_blue_on;		break;
			
		case 20:led_red_off;		led_blue_off;		RGB_UserDef.RGB_count = 0;		RGB_UserDef.APP_OrderFlag = false;		break;
	}
}


void LED_Indicate_Mode()
{//对应LED模式闪烁指示灯
	switch(RGB_UserDef.nrf_RGB_State)
	{
		case 0:
			LED_Reset_close();
			break;
		case 1://bluetooth未连接
			LED_Blue_Fast_Blink();
			break;
		case 2://bluetooth连接
			LED_Blue_Slow_Blink();
			break;
		case 3://正在充电
			LED_Red_Slow_Blink();
			break;
		case 4://充满电
			LED_Green_Open();
			break;
	}
}

















/* User code stop */
















































