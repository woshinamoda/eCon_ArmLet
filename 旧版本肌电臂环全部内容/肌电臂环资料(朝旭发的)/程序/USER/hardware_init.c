
#include "hardware_init.h"

//各个外设初始化
void HardWare_Init(void)
{ 	
	LED_Init();
  
    
	i2cInit();				  //初始化I2C总线
	delay_ms(10);			 //等待系统稳定
	
	init_mpu9150(); 		   //初始化传感器
	
	//get_compass_bias(); 	
  //compass_calibration();
		
	init_ADC();
	
	
}


