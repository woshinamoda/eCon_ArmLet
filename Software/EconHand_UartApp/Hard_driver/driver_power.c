#include "driver_power.h"

void EconHand_BSP_power_gpio_init()
{//EconHand��Դ����GPIO��ʼ��
	nrf_gpio_cfg_input(STA,NRF_GPIO_PIN_NOPULL);	
	nrf_gpio_cfg_output(AVDD_EN);
	nrf_gpio_cfg_output(DVDD_EN);
//	Control_DVDD(0);	//�޷��ٹرպ�˲��
	Control_DVDD(1);
	Control_AVDD(1);
}


//CPC4052 DVDD���֣�������Ҫ8��������
//CPC4052 DVDD���֣��ر���Ҫ����2ms����ʱ�ټ�1��������
void Control_DVDD(uint8_t state)
{
	if(state == 1)
	{
		for(uint8_t i=0; i<8; i++)
		{
			DVDD_L;
			nrf_delay_ms(1);
			DVDD_H;
			nrf_delay_ms(1);			
		}
	}
	else if(state == 0)
	{
		DVDD_L;
		nrf_delay_ms(10);	
		DVDD_H;
		nrf_delay_ms(1);		
		DVDD_L;		//�ڸ�λ�͵�ƽ��оƬʱ�̱�����͹������
	}
}

//TLV6122DBVR��TPS76350��ʹ��EN����һ��GPIO
void Control_AVDD(uint8_t	state)
{
	switch(state)
	{
		case 0: AVDD_L;	break;
		case 1:	AVDD_H;	break;
	}
}























































