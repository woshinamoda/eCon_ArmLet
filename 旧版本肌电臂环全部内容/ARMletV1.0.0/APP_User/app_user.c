/**********************************************************************
 * �������ļ�ʹ�ú���
 * ����-�ػ�-���ݴ���ʹ�ú���
 *
 * PB5 ----> Power_On
 *
 *
 * �߼�˼·
 * �������ж�
 *
 *
 *
 **********************************************************************/
#include <app_user.h>

void SysPower_Close()
{
	PBout(5) = 0;
}

void SysPower_Open()
{
	PBout(5) = 1;
}

//function: ���°����󿪻��жϺ���
// explain: ����ʱ���жϰ����Ƿ񳤰�
uint8_t KeyDown_Open_init()
{
	uint8_t Press_PA0_cnt = 0;				//��¼�������ڴ���
	uint8_t	Release_PA0_cnt = 0;			//��¼�ɿ����ڴ���
	while(1)
	{
		HAL_Delay(20);						//�ж�����ʱ��20ms
		if(PAin(0) == 1)					//��⵽��������
		{
			Press_PA0_cnt++;				//��¼����ʱ��
			Release_PA0_cnt = 0;			//�ᴥ�����ͷ�
			if(Press_PA0_cnt >= 50)
			{								//����ʱ�䳬��1sec
				return 1;					//����ֵ1
			}
		}
		else
		{
			Release_PA0_cnt++;
			if(Release_PA0_cnt > 5)
			{								//����5secû�а�
				return 0;					//����ֵ0
			}
		}
	}
}

//function: ����������жϺ���
// explain: ����MCU�ϵ�˲��ʹ��
uint8_t Charge_Open_init()
{
	uint8_t Press_PB4_cnt = 0;				//��¼�������ڴ���
	uint8_t	Release_PB4_cnt = 0;			//��¼�ɿ����ڴ���
	while(1)
	{
		HAL_Delay(20);						//�ж�����ʱ��20ms
		if(PBin(4) == 1)					//��⵽�����һֱ����2sec����
		{
			Press_PB4_cnt++;				//��¼����ʱ��
			Release_PB4_cnt = 0;			//�ᴥ�����ͷ�
			if(Press_PB4_cnt >= 50)
			{								//����ʱ�䳬��1sec
				return 1;					//����ֵ1
			}
		}
		else
		{
			Release_PB4_cnt++;
			if(Release_PB4_cnt > 5)
			{								//����5secû�а�
				return 0;					//����ֵ0
			}
		}
	}
}

//�жϿ������ų�ʼ��
void BSP_BP_Init()
{
   GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

//�жϿ��������ж�
void Open_WKUP_NVIC()
{
  /* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

//�����ж�
//fun��ֻ�г������߳�������볬�������Ժ������������
void WKUP_init()
{
	BSP_BP_Init();
	if((KeyDown_Open_init() == 0)&&(Charge_Open_init() == 0))
	{
		SysPower_Close();
	}
	SysPower_Open();
}






















