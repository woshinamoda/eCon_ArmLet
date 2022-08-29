
#include "stm32f4xx.h"
#include "stm_wkup.h"
#include "stm_sys.h"
#include "stm_delay.h"
#include "bsp_mpu.h"

#define WKUP_KD PAin(0)			//PA0 检测是否外部WK_UP按键按下

//系统进入待机模式
void Sys_Enter_Standby(void)
{		
  while(WKUP_KD);	
	
  mpu_set_dmp_state(1);
	//mpu_lp_motion_interrupt(200,1,10);	//配置传感器中断
	mpu_lp_motion_interrupt(600,20,10);	//配置传感器中断
	RCC_AHB1PeriphResetCmd(0X04FF,ENABLE);//复位所有IO口
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟		 
	 
	PWR_BackupAccessCmd(ENABLE);//后备区域访问使能

	PWR_ClearFlag(PWR_FLAG_WU);//清除Wake-up 标志
	 while(WKUP_KD);	
	PWR_WakeUpPinCmd(ENABLE);//设置WKUP用于唤醒
	
	PWR_EnterSTANDBYMode();	//进入待机模式
	 
}

short absd(short a,short b) 
{ 
short t; 
t=a-b; 
if(t<0) t*=-1; 
return t; 
}

