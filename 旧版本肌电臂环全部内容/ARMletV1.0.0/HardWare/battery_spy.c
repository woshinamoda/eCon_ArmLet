/*
 *	BAT-----|===|----------------|===|------GND
 * 			 10K	ADC(BAT)  	  20K
 *
 * 	��ѹ����ѡֵ����  10K - 10K  ��ѹ��1/2
 *
 *  ����ȡֵ��Χ����
 *  ����״̬��ѹ  3.8 ---  4.20V
 *  �еȵ�����ѹ  3.70 ---  3.80V
 *  �͵����µ�ѹ  Low  ---  3.70V
 *
 *  3.80/2/3.3*4096 �� 2350
 * 	3.70/2/3.3*4096 �� 2295
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
#define	N	10					//ƽ������������ֵ
uint16_t	BatValueBuf[N];		//����ADC���ݻ�������
uint8_t		index_B = 0;		//����ADCƽ��ֵ�ġ�ƽ������
uint16_t	sum = 0;			//��ص���ADC��ƽ��ֵ

uint16_t	UP_Mid = MidValue + 15;					//2365
uint16_t	DOWN_Mid = MidValue - 15;				//2335
uint16_t	UP_Low = LowValue + 15;					//2255
uint16_t	Down_Low = LowValue - 15;				//2225
uint16_t	LI_Old_state = 2;

//�������ܣ���10�ε�ص���ADC�����ݣ���ƽ��ֵ�����Ա�.
//		 �õ���ǰ���״̬
void Bat_Filter(void)
{
	sum = 0;
	BatValueBuf[index_B++] = (uint16_t)Li_battery;		//������������ض�ȡ��ص���ADC
	if(index_B == N)
	{//�����䵽������
		index_B = 0;
	     for(char count=0;count<N;count++)
		 {
	       sum += BatValueBuf[count];
		 }
	 sum/=N;	//ʵ��ÿ10�λ�ȡһ��sumֵ
	 Bat_Spy();
	}
}

//���ܺ�������ֵ��Χ�Ƚ��жϵ�ص���״̬
void Bat_Spy(void)
{
	uint16_t Li_Capacity = sum;		//����ƽ����ص���ADC�� �Ƚϱ���
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
	{//�ڿռ�ش�2335 - 2365   2225 - 2255���䣬����LL_Old_state�Ĳ���
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
		{//����س���
			if(Bat_Full_Cnt < 1000)
			{
				work_state = 2;	//1000�μ������Ϊδ����
				Bat_Full_Cnt++;
			}
			else
			{//�������1000�ε�������2.09		��Ϊ������
				Charge_Complete = 1;
			}
		}
		else
		{//δ����ʱ,����������
			work_state = 2;			//��⵽��س���ѹС��2.09��Ϊδ����	
			Bat_Full_Cnt = 0;		//������������0
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
		if(PBin(14) == 0)	//��⵽STAT(CHRG)���ŵ͵�ƽ����Ϊδ����	
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
















