/**********************************************************************
 *	关机逻辑
 *	关闭维持LDO高电平引脚
 *	关机过程会稍微震动一下，开电机和闪烁紫色灯200ms左右
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
	{//如果正常工作中触发了按键
		if(PBin(4) == 0)
		{//在确定充电器没有接入的情况下
			if(KeyDown_Open_init() == 1)
			{//如果长按2sec钟，确定有效按下
				SysPower_Close();	//关闭LDO维持EN的高电平
				Motor_Open();		//开电机
				Open_LED(5);		//开全色灯
				HAL_Delay(200);		//稍作延时
				Motor_Close();		//关电机
				Close_LED(5);		//关RGB灯
			}
		}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}
	if(GPIO_Pin == GPIO_PIN_4)
	{//检测到充电器拔出造成的下降沿
		if(Charge_Open_init() == 0)
		{//如果检测到100ms确实是低电平，说明是充电机彻底断开，那摩立刻进入关机状态

			Motor_Open();		//开电机
			Open_LED(5);		//开全色灯
			HAL_Delay(200);		//稍作延时
			Motor_Close();		//关电机
			Close_LED(5);		//关RGB灯
			SysPower_Close();	//关闭LDO维持EN的高电平
			//***brief***		用充电器关闭LDO是瞬间的，所以先执行效果显示，在断开维持LDO的高电平
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
	}

}
































