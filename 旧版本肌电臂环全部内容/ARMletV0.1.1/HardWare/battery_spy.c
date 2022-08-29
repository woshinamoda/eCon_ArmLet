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

uint16_t	UP_Mid = MidValue + 15;
uint16_t	DOWN_Mid = MidValue - 15;
uint16_t	UP_Low = LowValue + 15;
uint16_t	Down_Low = LowValue - 15;
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
	if((Li_Capacity >= UP_Mid)||((Li_Capacity > DOWN_Mid)&&(LI_Old_state == HighState)))
	{//���������ADC �����еȵ���+15  ���߳����еȵ���-15��ͬʱ��һ��״̬���Ǹߵ���״̬		��ô����״̬��Ϊ�ߵ���״̬
	if(LI_Old_state != HighState)
		LI_Old_state = HighState;
	}
	if(((Li_Capacity >= UP_Low)&&(Li_Capacity <= DOWN_Mid))||((Li_Capacity>Down_Low)&&(Li_Capacity<UP_Low)&&(LI_Old_state != Lowstate))||((Li_Capacity>DOWN_Mid)&&(Li_Capacity<UP_Mid)&&(LI_Old_state != HighState)))
	{
	if(LI_Old_state != MidState)
		LI_Old_state = MidState;
	}
	if((Li_Capacity <= Down_Low) || ((Li_Capacity < UP_Low)&&(LI_Old_state == Lowstate)))
	{//���������ADC С�ڵ͵ȵ���-15  ����С�ڵ͵ȵ���+15��ͬʱ��һ�λ��ǵ͵ȵ���		��ô����״̬��Ϊ�͵ȵ���
		if(LI_Old_state != Lowstate)
			LI_Old_state = Lowstate;
	}

}



















