
#include "stm32f4xx.h"
#include "stm_wkup.h"
#include "stm_sys.h"
#include "stm_delay.h"
#include "bsp_mpu.h"

#define WKUP_KD PAin(0)			//PA0 ����Ƿ��ⲿWK_UP��������

//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{		
  while(WKUP_KD);	
	
  mpu_set_dmp_state(1);
	//mpu_lp_motion_interrupt(200,1,10);	//���ô������ж�
	mpu_lp_motion_interrupt(600,20,10);	//���ô������ж�
	RCC_AHB1PeriphResetCmd(0X04FF,ENABLE);//��λ����IO��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��		 
	 
	PWR_BackupAccessCmd(ENABLE);//���������ʹ��

	PWR_ClearFlag(PWR_FLAG_WU);//���Wake-up ��־
	 while(WKUP_KD);	
	PWR_WakeUpPinCmd(ENABLE);//����WKUP���ڻ���
	
	PWR_EnterSTANDBYMode();	//�������ģʽ
	 
}

short absd(short a,short b) 
{ 
short t; 
t=a-b; 
if(t<0) t*=-1; 
return t; 
}

