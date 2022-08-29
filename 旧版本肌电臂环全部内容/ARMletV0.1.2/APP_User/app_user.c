/**********************************************************************
 * 主函数文件使用函数
 * 开机-关机-数据传输使用函数
 *
 * PB5 ----> Power_On
 *
 *
 * 逻辑思路
 * 开机先判断
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

//function: 按下按键后开机判断函数
// explain: 开机时候判断按键是否长按
uint8_t KeyDown_Open_init()
{
	uint8_t Press_PA0_cnt = 0;				//记录按下周期次数
	uint8_t	Release_PA0_cnt = 0;			//记录松开周期次数
	while(1)
	{
		HAL_Delay(20);						//判断周期时长20ms
		if(PAin(0) == 1)					//检测到按键按下
		{
			Press_PA0_cnt++;				//记录按下时长
			Release_PA0_cnt = 0;			//轻触按键释放
			if(Press_PA0_cnt >= 50)
			{								//按下时间超过1sec
				return 1;					//返回值1
			}
		}
		else
		{
			Release_PA0_cnt++;
			if(Release_PA0_cnt > 5)
			{								//超过5sec没有按
				return 0;					//返回值0
			}
		}
	}
}

//function: 充电器开机判断函数
// explain: 仅仅MCU上电瞬间使用
uint8_t Charge_Open_init()
{
	uint8_t Press_PB4_cnt = 0;				//记录按下周期次数
	uint8_t	Release_PB4_cnt = 0;			//记录松开周期次数
	while(1)
	{
		HAL_Delay(20);						//判断周期时长20ms
		if(PBin(4) == 1)					//检测到充电器一直插着2sec以上
		{
			Press_PB4_cnt++;				//记录按下时长
			Release_PB4_cnt = 0;			//轻触按键释放
			if(Press_PB4_cnt >= 50)
			{								//按下时间超过1sec
				return 1;					//返回值1
			}
		}
		else
		{
			Release_PB4_cnt++;
			if(Release_PB4_cnt > 5)
			{								//超过5sec没有按
				return 0;					//返回值0
			}
		}
	}
}

//判断开机引脚初始化
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

//判断开机引脚中断
void Open_WKUP_NVIC()
{
  /* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

//阻塞判断
//fun：只有长按或者充电器插入超过两秒以后才能正常开机
void WKUP_init()
{
	BSP_BP_Init();
	if((KeyDown_Open_init() == 0)&&(Charge_Open_init() == 0))
	{
		SysPower_Close();
	}
	SysPower_Open();
}






















