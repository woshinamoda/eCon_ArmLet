/**********************************************************************
 *	�ػ��߼�
 *	�ر�ά��LDO�ߵ�ƽ����
 *	�ػ����̻���΢��һ�£����������˸��ɫ��200ms����
 *
 *
 *
 *
 *
 **********************************************************************/
#include <exit_work.h>
#include "stm32f4xx_hal.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{//������������д����˰���
		if(PBin(4) == 0)
		{//��ȷ�������û�н���������
			if(KeyDown_Open_init() == 1)
			{//�������2sec�ӣ�ȷ����Ч����
				SysPower_Close();	//�ر�LDOά��EN�ĸߵ�ƽ
				Motor_Open();		//�����
				Open_LED(5);		//��ȫɫ��
				HAL_Delay(200);		//������ʱ
				Motor_Close();		//�ص��
				Close_LED(5);		//��RGB��
			}
		}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}
	if(GPIO_Pin == GPIO_PIN_4)
	{//��⵽������γ���ɵ��½���
		if(Charge_Open_init() == 0)
		{//�����⵽100msȷʵ�ǵ͵�ƽ��˵���ǳ������׶Ͽ�����Ħ���̽���ػ�״̬

			Motor_Open();		//�����
			Open_LED(5);		//��ȫɫ��
			HAL_Delay(200);		//������ʱ
			Motor_Close();		//�ص��
			Close_LED(5);		//��RGB��
			SysPower_Close();	//�ر�LDOά��EN�ĸߵ�ƽ
			//***brief***		�ó�����ر�LDO��˲��ģ�������ִ��Ч����ʾ���ڶϿ�ά��LDO�ĸߵ�ƽ
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
	}

}
































