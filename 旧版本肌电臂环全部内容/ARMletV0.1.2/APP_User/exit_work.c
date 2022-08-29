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


extern	uint8_t work_state;				//工作状态变量	1：充电充满  2：正在充电	3：正常工作	  4：电量不足  5：电量微弱请及时充电
extern	uint8_t Booth_state;			//蓝牙连接状态  0：未连接   1：连接状态
extern	uint8_t Charge_state;			//充电状态		0：正在充   1：已经充满
extern	uint8_t htim9_flag;				//定时器9旗标
extern	uint8_t htim10_flag;			//定时器10旗标
extern	uint8_t TimeFlag;				//EMG中断旗标
extern	uint8_t IMUFlag;				//IMU中断旗标
extern	uint8_t SystemFlag;				//完成初始化旗标

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{//如果正常工作中触发了按键
		if(PBin(4) == 0)
		{//在确定充电器没有接入的情况下
			if(KeyDown_Open_init() == 1)
			{//如果长按2sec钟，确定有效按下
				HAL_TIM_Base_Stop_IT(&htim3);
				HAL_TIM_Base_Stop_IT(&htim11);			//关闭两个点灯函数
				Motor_Open();		//开电机
				Open_LED(5);		//开全色灯
				PBout(13) = 0;		//开蓝牙灯
				HAL_Delay(300);		//稍作延时
				Motor_Close();		//关电机
				Close_LED(5);		//关RGB灯
				PBout(13) = 1;		//关闭蓝牙灯
				SysPower_Close();	//关闭LDO维持EN的高电平
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
	if(GPIO_Pin == GPIO_PIN_2)
	{//检测到是姿态传感器的触发中断
		IMUFlag = 1;
		Read_DMP();
		IMU_Data_Send();
	}

}

void htim3_work_serve()
{
	if(PBin(4) != 1)
	{//如果没接充电器，正常显示工作
		switch(Booth_state)
		{
		case 1://如果蓝牙没有连接，循环反转IO口使其闪烁
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
			break;
		case 0://如果蓝牙处于连接状态，直接高低平使其常量
			PBout(13) = 0;
			break;
		}
	}
	else
	{//如果接了充电器，直接关闭蓝牙指示灯
		PBout(13) = 1;
	}
	if(PBin(4) == 1)
	{//只有在接入充电器的情况下
		switch(Charge_state)
		{
		case 0://如果没有充满，绿色闪烁
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
			break;
		case 1://如果充满绿色常量
			PBout(6) = 0;
			break;
		}
	}
}

void htim9_work_serve()
{
	if(LI_Old_state == 2)
	work_state = 3;		//正常状态下是蓝色常量---优先级最低
	if(LI_Old_state == 1)
	work_state = 4;		//中等电量黄色常量
	if(LI_Old_state == 0)
	work_state = 5;		//低等电量红色常量
	if(PBin(4) == 1)
	{//如果接入充电器状态
		if(SystemFlag==1)
		{//确定完成初始化以后方可在充电时关闭蓝牙电源和LED
			PCout(14) = 0;	//因为只要拔掉充电器就会重启，默认关掉所有
			//HAL_TIM_Base_Stop_IT(&htim10);	
		}
		switch(PAin(12))
		{
		case 0:		//CHRG = 0 电池正在充电
			work_state = 2;
			break;
		case 1:		//CHRG = 1 电池充满
			work_state = 1;
			break;
		}
	}//end of PB4 judege
	else
	{//如果没有接入充电器


	}
	if(PBin(12) == 1)
	{//蓝牙处于连接状态
		Booth_state = 1;
	}
	else
	{//蓝牙处于断开状态
		Booth_state = 0;
	}//end of PB12 judge
}

void htim11_work_serve()
{
	Bat_Filter();	//电池电量判断函数
	switch(work_state)
	{
	case 1:
		Charge_state = 1;	//充满电
		Close_LED(2);
		Close_LED(3);		//其他定时器里用绿色反转，就不要全部关闭，以免干扰
		break;
	case 2:
		Charge_state = 0;	//正在充电
		Close_LED(2);
		Close_LED(3);		//其他定时器里用绿色反转，就不要全部关闭，以免干扰
		break;
	case 3:
		Close_LED(0);
		Open_LED(2);		//正常状态开蓝色
		break;
	case 4:
		Close_LED(0);
		Open_LED(4);		//电量中等开黄灯
		break;
	case 5:
		Close_LED(0);
		Open_LED(3);		//电量微弱，开红灯
		break;
	}
}


























