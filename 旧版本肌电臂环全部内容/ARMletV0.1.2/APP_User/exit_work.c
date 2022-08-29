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


extern	uint8_t work_state;				//����״̬����	1��������  2�����ڳ��	3����������	  4����������  5������΢���뼰ʱ���
extern	uint8_t Booth_state;			//��������״̬  0��δ����   1������״̬
extern	uint8_t Charge_state;			//���״̬		0�����ڳ�   1���Ѿ�����
extern	uint8_t htim9_flag;				//��ʱ��9���
extern	uint8_t htim10_flag;			//��ʱ��10���
extern	uint8_t TimeFlag;				//EMG�ж����
extern	uint8_t IMUFlag;				//IMU�ж����
extern	uint8_t SystemFlag;				//��ɳ�ʼ�����

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{//������������д����˰���
		if(PBin(4) == 0)
		{//��ȷ�������û�н���������
			if(KeyDown_Open_init() == 1)
			{//�������2sec�ӣ�ȷ����Ч����
				HAL_TIM_Base_Stop_IT(&htim3);
				HAL_TIM_Base_Stop_IT(&htim11);			//�ر�������ƺ���
				Motor_Open();		//�����
				Open_LED(5);		//��ȫɫ��
				PBout(13) = 0;		//��������
				HAL_Delay(300);		//������ʱ
				Motor_Close();		//�ص��
				Close_LED(5);		//��RGB��
				PBout(13) = 1;		//�ر�������
				SysPower_Close();	//�ر�LDOά��EN�ĸߵ�ƽ
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
	if(GPIO_Pin == GPIO_PIN_2)
	{//��⵽����̬�������Ĵ����ж�
		IMUFlag = 1;
		Read_DMP();
		IMU_Data_Send();
	}

}

void htim3_work_serve()
{
	if(PBin(4) != 1)
	{//���û�ӳ������������ʾ����
		switch(Booth_state)
		{
		case 1://�������û�����ӣ�ѭ����תIO��ʹ����˸
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
			break;
		case 0://���������������״̬��ֱ�Ӹߵ�ƽʹ�䳣��
			PBout(13) = 0;
			break;
		}
	}
	else
	{//������˳������ֱ�ӹر�����ָʾ��
		PBout(13) = 1;
	}
	if(PBin(4) == 1)
	{//ֻ���ڽ��������������
		switch(Charge_state)
		{
		case 0://���û�г�������ɫ��˸
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
			break;
		case 1://���������ɫ����
			PBout(6) = 0;
			break;
		}
	}
}

void htim9_work_serve()
{
	if(LI_Old_state == 2)
	work_state = 3;		//����״̬������ɫ����---���ȼ����
	if(LI_Old_state == 1)
	work_state = 4;		//�еȵ�����ɫ����
	if(LI_Old_state == 0)
	work_state = 5;		//�͵ȵ�����ɫ����
	if(PBin(4) == 1)
	{//�����������״̬
		if(SystemFlag==1)
		{//ȷ����ɳ�ʼ���Ժ󷽿��ڳ��ʱ�ر�������Դ��LED
			PCout(14) = 0;	//��ΪֻҪ�ε�������ͻ�������Ĭ�Ϲص�����
			//HAL_TIM_Base_Stop_IT(&htim10);	
		}
		switch(PAin(12))
		{
		case 0:		//CHRG = 0 ������ڳ��
			work_state = 2;
			break;
		case 1:		//CHRG = 1 ��س���
			work_state = 1;
			break;
		}
	}//end of PB4 judege
	else
	{//���û�н�������


	}
	if(PBin(12) == 1)
	{//������������״̬
		Booth_state = 1;
	}
	else
	{//�������ڶϿ�״̬
		Booth_state = 0;
	}//end of PB12 judge
}

void htim11_work_serve()
{
	Bat_Filter();	//��ص����жϺ���
	switch(work_state)
	{
	case 1:
		Charge_state = 1;	//������
		Close_LED(2);
		Close_LED(3);		//������ʱ��������ɫ��ת���Ͳ�Ҫȫ���رգ��������
		break;
	case 2:
		Charge_state = 0;	//���ڳ��
		Close_LED(2);
		Close_LED(3);		//������ʱ��������ɫ��ת���Ͳ�Ҫȫ���رգ��������
		break;
	case 3:
		Close_LED(0);
		Open_LED(2);		//����״̬����ɫ
		break;
	case 4:
		Close_LED(0);
		Open_LED(4);		//�����еȿ��Ƶ�
		break;
	case 5:
		Close_LED(0);
		Open_LED(3);		//����΢���������
		break;
	}
}


























