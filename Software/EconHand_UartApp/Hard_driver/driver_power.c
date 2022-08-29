#include "driver_power.h"

void EconHand_BSP_power_gpio_init()
{//EconHand电源管理GPIO初始化
	nrf_gpio_cfg_input(STA,NRF_GPIO_PIN_NOPULL);	
	nrf_gpio_cfg_output(AVDD_EN);
	nrf_gpio_cfg_output(DVDD_EN);
//	Control_DVDD(0);	//无法再关闭后瞬开
	Control_DVDD(1);
	Control_AVDD(1);
}


//CPC4052 DVDD部分，开启需要8个上升沿
//CPC4052 DVDD部分，关闭需要超过2ms的延时再加1个上升沿
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
		DVDD_L;		//在复位低电平，芯片时刻保持最低功耗输出
	}
}

//TLV6122DBVR和TPS76350的使能EN公用一个GPIO
void Control_AVDD(uint8_t	state)
{
	switch(state)
	{
		case 0: AVDD_L;	break;
		case 1:	AVDD_H;	break;
	}
}























































