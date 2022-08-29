/*
 *	BAT-----|===|----------------|===|------GND
 * 			 10K	ADC(BAT)  	  20K
 *
 * 	分压电阻选值如上  10K - 10K  分压比1/2
 *
 *  量程取值范围如下
 *  正常状态电压  3.85 ---  4.20V
 *  中等电量电压  3.70 ---  3.85V
 *  低电量下电压  Low  ---  3.70V
 *
 *  3.85/2/3.3*4096 ≈ 2390
 * 	3.70/2/3.3*4096 ≈ 2295
 *
 */

#include "battery_spy.h"
#include "led.h"
#include "usart.h"
#include "adc_data.h"

#define	HighState	2
#define MidState	1
#define Lowstate	0

#define MidValue	2390	//3.85V == Mid Power
#define	LowValue	2295	//3.70V == Low Power


/***********Bat_filter**********/
#define	N	10					//平均数组个数最大值
uint16_t	BatValueBuf[N];		//电量ADC数据缓冲数组
uint8_t		index_B = 0;		//测量ADC平均值的“平均数”
uint16_t	sum = 0;			//电池电量ADC的平均值

uint16_t	UP_Mid = MidValue + 15;
uint16_t	DOWN_Mid = MidValue - 15;
uint16_t	UP_Low = LowValue + 15;
uint16_t	Down_Low = LowValue - 15;
uint16_t	LI_Old_state;

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
	if((Li_Capacity >= UP_Mid)||((Li_Capacity >= DOWN_Mid)&&(LI_Old_state == HighState)))
	{//如果读到的ADC 超过中等电量+15  或者超过中等电量-15，同时上一次状态还是高电量状态		那么本次状态视为高电量状态
		LI_Old_state == HighState;
	}
	if(((Li_Capacity >= UP_Low)&&(Li_Capacity < UP_Mid))||((Li_Capacity >= Down_Low)&&(LI_Old_state == MidState)))
	{//如果独到的ADC 超过低等电量+15且小于高等电量-15  或者超过低等电量-15，同时上一次状态还是中等电量		那么本次视为中等电量
		LI_Old_state == MidState;
	}
	if((Li_Capacity <= Down_Low) || ((Li_Capacity < UP_Low)&&(LI_Old_state == Lowstate)))
	{//如果读到的ADC 小于低等电量-15  或者小于低等电量+15，同时上一次还是低等电量		那么本次状态视为低等电量
		LI_Old_state == Lowstate;
	}

}



















