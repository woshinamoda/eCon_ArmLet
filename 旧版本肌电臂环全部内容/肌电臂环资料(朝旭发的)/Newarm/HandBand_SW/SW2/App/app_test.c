#include "app_test.h"
#include "tim.h"
#include "usart.h"
#include "adc_data.h"
#include "battery_spy.h"
#include "string.h"
#include "motor.h"
#include "led.h"
#include "mpu6050.h"
#include "inv_mpu.h"

uint8_t TimeFlag = 0;
uint8_t MPU_Flag = 0;
/*****************EMG & Battery*********************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==htim10.Instance)
	{
		TimeFlag = 1;
		get_ADCdata();
	}
	if(htim->Instance==htim11.Instance)
	{
		MPU_Flag = 1;
		Bat_Filter();
	}
}

uint8_t RxCount=0;
uint8_t RxNum=0;
uint8_t RxBuffer[1];
uint8_t ReceiveBytes[8]={0};
uint8_t CommandRecevFlag=0;
/*****************中断接收命令，trigger功能，CommandRecevFlag*********************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		ReceiveBytes[RxCount] = RxBuffer[0];
		if( ((ReceiveBytes[RxCount-1]==0x0D)&&(ReceiveBytes[RxCount]==0x0A)) || (RxCount >= 7) )
		{
			CommandRecevFlag = 1;
			RxNum = ++RxCount;
			RxCount=0;
		}
		else RxCount++;
		HAL_UART_Receive_IT(&huart1,RxBuffer,1);
	}
}

/***********************************************/
void StandbyMode_Measure(void)
{  

      /* Enable Power Clock*/
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Allow access to Backup */
    HAL_PWR_EnableBkUpAccess();

      /* Reset RTC Domain */
    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();


    /* Disable all used wakeup sources: Pin1(PA.0) */
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Re-enable all used wakeup sources: Pin1(PA.0) */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    /*## Enter Standby Mode ####################################################*/
    /* Request to enter STANDBY mode  */
    HAL_PWR_EnterSTANDBYMode(); 
  
}


void Sys_Enter_Standby(void)
{
	
    //关闭所有外设(根据实际情况写)
    __HAL_RCC_APB2_FORCE_RESET();//
    //  __HAL_RCC_GPIOC_CLK_DISABLE();
    //__HAL_RCC_GPIOD_CLK_DISABLE();
    //__HAL_RCC_GPIOA_CLK_DISABLE();
    //__HAL_RCC_GPIOB_CLK_DISABLE();
	
    
    StandbyMode_Measure();//进入待机模式
}
/************************************************************/

void Main_Init(void)
{
//	MPU_Init();
//	mpu_lp_motion_interrupt(600,20,10);	//配置传感器中断

//	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))//充电的时候进入休眠
//	{
//		Sys_Enter_Standby();
//	}
	ADCStart();
//	HAL_TIM_Base_Start_IT(&htim10);//定时器中断开启
//	HAL_UART_Receive_IT(&huart1,RxBuffer,1);	
//	HAL_Delay(10);//wait battery value stable
//	HAL_TIM_Base_Start_IT(&htim11);//battery定时器中断开启
}
	
short ax, ay, az;
short tempDiff,Wkup_store;
uint16_t Wkup_cont = 0;
uint8_t Wkup_Flag = 0;
#define ABS(a,b)   ((a-b)>0?(a-b):(b-a))



void Mainloop(void)
{
	
		if(TimeFlag)
		{
			TimeFlag = 0;
			/* set trigger*/
			if((ReceiveBytes[0] == 'T') && (ReceiveBytes[1] == 'G') && (ReceiveBytes[2] == 0x0D) && (ReceiveBytes[3] == 0x0A))
			{
				memset(ReceiveBytes,0,8);
				Emg_Data_Sent[15] = 0xFF;			
			}	
			/* set trigger*/
			HAL_UART_Transmit_DMA(&huart1, Emg_Data_Sent, 16);
		}
		/*********enter Standby mode************/
//		if(MPU_Flag == 1)
//		{
//			MPU_Flag = 0;
//			MPU_Get_Accelerometer(&ax,&ay,&az);//会占用时间
//			tempDiff=ABS(Wkup_store,ax);
//			if(tempDiff<500) /*Motion threshold*/
//				Wkup_cont+=1;
//			else 
//			{
//				Wkup_cont=0;
//				Wkup_Flag=0;
//			}
//			Wkup_store = ax;
//			if(Wkup_cont>20*60*10)/*20Hz,60s,10min,set time to standby*/
//				Wkup_Flag=1;
//			if(Wkup_Flag)
//			{
////				Motor_Short_Soft();
//				Sys_Enter_Standby();
//			}	
//		}
}
	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_15)//PB15
    {
			HAL_Delay(20);
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))
			{
				LED_close();
//				Motor_Short_Strong();
				Sys_Enter_Standby();			
			}

    }    
}
	


