#include "driver_key.h"
#include "nrf.h"
#include "app_timer.h"




/* 中断判断按键是否触发函数	*/
void Key_Botton_Handle()
{//如果按键按下
	if(nrf_gpio_pin_read(PWOER_KEY_PIN)==0)
	{//如果检测到开关按下
		nrf_delay_ms(10);
		if(nrf_gpio_pin_read(PWOER_KEY_PIN)==0)
		{//确实按键被按下
			Key_UserDef.Key_Botton_state = true;
			Key_UserDef.Key_BottonTime_count = 0;		
		}
	}
	else
	{//如果检测到按键松开（中断是双边沿检测）
		Key_UserDef.Key_Botton_state = false;
		Key_UserDef.Key_BottonTime_count = 0;			//松开计数清零，等待下次进来重新计算
	}
}



/* 定时器判断*/
void	Key_Time_JudgeOnOff()
{
	if(Key_UserDef.Key_Botton_state == true)
	{
		Key_UserDef.Key_BottonTime_count++;
		if((Key_UserDef.Key_BottonTime_count > 1) && (Key_UserDef.Key_BottonTime_count <50))
		{//按键短按生效
			
		}
		if(Key_UserDef.Key_BottonTime_count > 200)
		{//长按按键生效
			
			LED_Reset_close();	
			led_red_on;									
			nrf_delay_ms(900);
			led_red_off;									
			nrf_gpio_cfg_default(27);		
			while(nrf_gpio_pin_read(PWOER_KEY_PIN)==1)
			{//确实按键弹起
				Sleep_withOut_red();
			}		
		}
	}
}


/* Vcheck充电器接入检测 */
void	Vcheck_Handle()
{
	if(nrf_gpio_pin_read(Vcheck_PIN)==1)
	{//检测到插入充电器
		nrf_delay_ms(10);
		if(nrf_gpio_pin_read(Vcheck_PIN)==1)
		{//确实接入充电器
			Vcheck_state = true;		
			APP_Close_BleADV();									//主动断开蓝牙和广播
			Observe_Close_BleSoftDevice();			//如果主动断开的是蓝牙，那么设置广播不重启/
			LED_Reset_close();
			RGB_UserDef.nrf_RGB_State = 3;			 
		}
	}
	else if(nrf_gpio_pin_read(Vcheck_PIN)==0)
	{//检测充电器接入口是低电平
		if(Vcheck_state == true)
		{//首先确认之前充电器是接着的
			nrf_delay_ms(200);
			if(nrf_gpio_pin_read(Vcheck_PIN)==0)
			{//确实拔下充电器
				Vcheck_state = false;	
				sleep_mode_enter();
			}
		}
		else{};	//没有接入充电器或者干扰脉冲，不做判断
	}
}









































