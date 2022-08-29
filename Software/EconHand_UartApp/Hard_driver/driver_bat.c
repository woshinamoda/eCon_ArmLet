#include "driver_bat.h"
#include "ble_bas.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"



/*----------------------------------------------------------------------
�����ٷֱȼ��㺯��
0%	:		3.2 > data								1600 >  data
10% :		3.3 >	data	> 3.2					1650 >  data	> 1600						
20% :		3.4 >	data	> 3.3				  1700 >  data	> 1650											
30% :		3.5 >	data	> 3.4				  1750 >  data	> 1700	
40% :		3.6 >	data	> 3.5					1800 >  data	> 1750
50% :		3.7 >	data	> 3.6					1850 >  data	> 1800
60% :		3.8 >	data	> 3.7					1900 >  data	> 1850
70% :		3.9 >	data	> 3.8					1950 >  data	> 1900
80% :		4.0 >	data	> 3.9					2000 >  data	> 1950
90% :		4.1 >	data	> 4.0				  2050 >  data	> 2000
100%:   			data	> 4.1									data	> 2050


	��Ӧÿ����λ����20�������ռ�
ADC��Ӧ��ذٷֱ�����
						|NC
	100
						|2060
=======================20�Ĳ����ռ�
						|2040
	90
						|2010
=======================20�Ĳ����ռ�						
						|1990
	80
						|1960
=======================20�Ĳ����ռ�						
						|1940
	70
						|1910
=======================20�Ĳ����ռ�
						|1890
	60
						|1860
=======================20�Ĳ����ռ�						
						|1840
	50
						|1810
=======================20�Ĳ����ռ�
						|1790
	40
						|1760
=======================20�Ĳ����ռ�	
						|1740
	30
						|1710
=======================20�Ĳ����ռ�
						|1690
	20
						|1660
=======================20�Ĳ����ռ�						
						|1640
	10
						|1610
=======================20�Ĳ����ռ�
						|1590
	0
						|NC
				
---------------------------------------------------------------------*/
uint8_t	User_Set_battery_level_10per(uint16_t mvolts, uint8_t las_level)
{//ADCֵת�ٷֱ�
	uint8_t	bat_level;
	if(mvolts >= 2060)
		bat_level = 100;
	
	else if((mvolts <= 2040)&&(mvolts >= 2010))
		bat_level = 90;	

	else if((mvolts <= 1990)&&(mvolts >= 1960))
		bat_level = 80;
	
	else if((mvolts <= 1940)&&(mvolts >= 1910))
		bat_level = 70;
	
	else if((mvolts <= 1890)&&(mvolts >= 1860))
		bat_level = 60;
	
	else if((mvolts <= 1840)&&(mvolts >= 1810))
		bat_level = 50;
	
	else if((mvolts <= 1790)&&(mvolts >= 1760))
		bat_level = 40;
	
	else if((mvolts <= 1740)&&(mvolts >= 1710))
		bat_level = 30;
	
	else if((mvolts <= 1690)&&(mvolts >= 1660))
		bat_level = 20;
	
	else if((mvolts <= 1640)&&(mvolts >= 1610))
		bat_level = 10;
	
	else if((mvolts <= 1590))
		bat_level = 0;
	
	else
		bat_level = las_level;	//��������ڷ�Χ�ڣ��ͱ�����һ�ζ�ȡ�ķ�Χֵ
	
	return bat_level;
}

uint8_t	First_battery_level(uint16_t mvolts)
{//��һ�βɼ�ADC����ĵ�ذٷֱ�
	uint8_t	bat_level;
	if(mvolts >= 2050)
		bat_level = 100;
	
	else if((mvolts < 2050)&&(mvolts >= 2000))
		bat_level = 90;	

	else if((mvolts < 2000)&&(mvolts >= 1950))
		bat_level = 80;
	
	else if((mvolts < 1950)&&(mvolts >= 1900))
		bat_level = 70;
	
	else if((mvolts < 1900)&&(mvolts >= 1850))
		bat_level = 60;
	
	else if((mvolts < 1850)&&(mvolts >= 1800))
		bat_level = 50;
	
	else if((mvolts < 1800)&&(mvolts >= 1750))
		bat_level = 40;
	
	else if((mvolts < 1750)&&(mvolts >= 1700))
		bat_level = 30;
	
	else if((mvolts < 1700)&&(mvolts >= 1650))
		bat_level = 20;
	
	else if((mvolts <= 1650)&&(mvolts >= 1600))
		bat_level = 10;
	
	else if((mvolts < 1600))
		bat_level = 0;

		return bat_level;
}


void BatdataBubbleSort(uint16_t * p_bat_data, uint8_t length)
{//ð���㷨��Ϊƽ���˲���׼��
	uint8_t i,j;
	uint16_t max;
	uint8_t len;
	len=length-1;
	for(i=0;i<len;i++)
	{
		for(j=0;j<(len-i);j++)
		{//length�����������Ա�֤�����������������С
			if(p_bat_data[j]>p_bat_data[j+1])
			{//1�����������Ա�֤����ֵ�����ұ�
				max=p_bat_data[j+1];
				p_bat_data[j+1]=p_bat_data[j];
				p_bat_data[j]=max;
				//�Ƚ����ڵ�Ԫ�أ���ֵ���Ԫ�ؽ������ұ�
			}	
		}
	}
}


uint16_t	GetBatAvr(uint16_t * p_bat_data, uint8_t length)
{//����ƽ��ֵ��ȥ�����ֵ����Сֵ
	uint8_t	i = 0;
	uint16_t	sum = 0;
	if(length < 3)
	{
		sum=p_bat_data[0]+p_bat_data[1];
		return (sum/2);	
	}
	else
	{
		for(i = 0;i < (length - 2);i++)
		{
			sum += p_bat_data[i+1];
		}
	}
	return (sum/(length - 2));
}



































