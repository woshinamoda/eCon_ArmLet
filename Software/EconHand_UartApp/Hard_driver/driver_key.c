#include "driver_key.h"
#include "nrf.h"
#include "app_timer.h"




/* �ж��жϰ����Ƿ񴥷�����	*/
void Key_Botton_Handle()
{//�����������
	if(nrf_gpio_pin_read(PWOER_KEY_PIN)==0)
	{//�����⵽���ذ���
		nrf_delay_ms(10);
		if(nrf_gpio_pin_read(PWOER_KEY_PIN)==0)
		{//ȷʵ����������
			Key_UserDef.Key_Botton_state = true;
			Key_UserDef.Key_BottonTime_count = 0;		
		}
	}
	else
	{//�����⵽�����ɿ����ж���˫���ؼ�⣩
		Key_UserDef.Key_Botton_state = false;
		Key_UserDef.Key_BottonTime_count = 0;			//�ɿ��������㣬�ȴ��´ν������¼���
	}
}



/* ��ʱ���ж�*/
void	Key_Time_JudgeOnOff()
{
	if(Key_UserDef.Key_Botton_state == true)
	{
		Key_UserDef.Key_BottonTime_count++;
		if((Key_UserDef.Key_BottonTime_count > 1) && (Key_UserDef.Key_BottonTime_count <50))
		{//�����̰���Ч
			
		}
		if(Key_UserDef.Key_BottonTime_count > 200)
		{//����������Ч
			
			LED_Reset_close();	
			led_red_on;									
			nrf_delay_ms(900);
			led_red_off;									
			nrf_gpio_cfg_default(27);		
			while(nrf_gpio_pin_read(PWOER_KEY_PIN)==1)
			{//ȷʵ��������
				Sleep_withOut_red();
			}		
		}
	}
}


/* Vcheck����������� */
void	Vcheck_Handle()
{
	if(nrf_gpio_pin_read(Vcheck_PIN)==1)
	{//��⵽��������
		nrf_delay_ms(10);
		if(nrf_gpio_pin_read(Vcheck_PIN)==1)
		{//ȷʵ��������
			Vcheck_state = true;		
			APP_Close_BleADV();									//�����Ͽ������͹㲥
			Observe_Close_BleSoftDevice();			//��������Ͽ�������������ô���ù㲥������/
			LED_Reset_close();
			RGB_UserDef.nrf_RGB_State = 3;			 
		}
	}
	else if(nrf_gpio_pin_read(Vcheck_PIN)==0)
	{//�������������ǵ͵�ƽ
		if(Vcheck_state == true)
		{//����ȷ��֮ǰ������ǽ��ŵ�
			nrf_delay_ms(200);
			if(nrf_gpio_pin_read(Vcheck_PIN)==0)
			{//ȷʵ���³����
				Vcheck_state = false;	
				sleep_mode_enter();
			}
		}
		else{};	//û�н����������߸������壬�����ж�
	}
}









































