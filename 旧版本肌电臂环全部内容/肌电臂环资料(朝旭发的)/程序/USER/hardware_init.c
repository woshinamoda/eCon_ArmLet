
#include "hardware_init.h"

//���������ʼ��
void HardWare_Init(void)
{ 	
	LED_Init();
  
    
	i2cInit();				  //��ʼ��I2C����
	delay_ms(10);			 //�ȴ�ϵͳ�ȶ�
	
	init_mpu9150(); 		   //��ʼ��������
	
	//get_compass_bias(); 	
  //compass_calibration();
		
	init_ADC();
	
	
}


