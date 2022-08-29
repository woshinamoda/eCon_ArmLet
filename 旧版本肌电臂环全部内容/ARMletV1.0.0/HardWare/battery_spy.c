/*
 *	BAT-----|===|----------------|===|------GND
 * 			 10K	ADC(BAT)  	  20K
 *
 * 	分压电阻选值如上  10K - 10K  分压比1/2
 *
 *  量程取值范围如下
 *  正常状态电压  3.8 ---  4.20V
 *  中等电量电压  3.70 ---  3.80V
 *  低电量下电压  Low  ---  3.70V
 *
 *  3.80/2/3.3*4096 ≈ 2350
 * 	3.70/2/3.3*4096 ≈ 2295
 *
 */

#include "battery_spy.h"
#include "led.h"
#include "usart.h"
#include "adc_data.h"
#include "main.h"

#define	HighState	2
#define MidState	1
#define Lowstate	0

#define MidValue	2350	//3.85V == Mid Power
#define	LowValue	2240	//3.65V == Low Power


/***********Bat_filter**********/
#define	N	10					//平均数组个数最大值
uint16_t	BatValueBuf[N];		//电量ADC数据缓冲数组
uint8_t		index_B = 0;		//测量ADC平均值的“平均数”
uint16_t	sum = 0;			//电池电量ADC的平均值

uint16_t	UP_Mid = MidValue + 15;					//2365
uint16_t	DOWN_Mid = MidValue - 15;				//2335
uint16_t	UP_Low = LowValue + 15;					//2255
uint16_t	Down_Low = LowValue - 15;				//2225
uint16_t	LI_Old_state = 2;

//函数功能：求10次电池电量ADC的数据，求平均值并作对比.
//		 得到当前电池状态
void Bat_Filter(void)
{
	sum = 0;
	BatValueBuf[index_B++] = (uint16_t)Li_battery;		//向数组内填充电池读取电池电量ADC
	if(index_B == N)
	{//如果填充到达上线
		index_B = 0;
	     for(char count=0;count<N;count++)
		 {
	       sum += BatValueBuf[count];
		 }
	 sum/=N;	//实际每10次获取一个sum值
	 Bat_Spy();
	}
}

//功能函数：阈值范围比较判断电池电量状态
void Bat_Spy(void)
{
	uint16_t Li_Capacity = sum;		//传递平均电池电量ADC给 比较变量
	if(Li_Capacity >= UP_Mid)
	{//	2365	 <  Li_Capacity
		LI_Old_state = HighState;
	}
	else if((Li_Capacity >= UP_Low)&&(Li_Capacity <= DOWN_Mid))
	{//	2255	 <  Li_Capacity		<		2335
		LI_Old_state = MidState;
	}
	else if(Li_Capacity <= Down_Low)
	{//						Li_Capacity		<		2225
		LI_Old_state = Lowstate;
	}
	else
	{//在空间地带2335 - 2365   2225 - 2255区间，保持LL_Old_state的不变
		LI_Old_state = LI_Old_state;
	}
}

uint16_t	Bat_Full_Cnt;
uint8_t		Charge_Complete = 0;


void	check_Charge_OK()
{
	if(Charge_Complete == 0)
	{
		if(Li_battery>2600) //2.09/3.3*4096  
		{//检测电池充满
			if(Bat_Full_Cnt < 1000)
			{
				work_state = 2;	//1000次检测内视为未充满
				Bat_Full_Cnt++;
			}
			else
			{//连续检测1000次电量大于2.09		视为充满电
				Charge_Complete = 1;
			}
		}
		else
		{//未充满时,充电计数清零
			work_state = 2;			//检测到电池充电电压小于2.09视为未充满	
			Bat_Full_Cnt = 0;		//充电检测计数器清0
		}
	}
	else
	{
		work_state = 1;	
	}
}

void check_Charge_BySTAT()
{
	if(Charge_Complete == 0)
	{
		if(PBin(14) == 0)	//检测到STAT(CHRG)引脚低电平，视为未充满	
		{
			work_state = 2;		
		}
		else
		{
			Charge_Complete = 1;		
		}
	}
	else
	{
		work_state = 1;
	}
}
















