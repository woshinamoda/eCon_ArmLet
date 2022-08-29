/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/*============================肌电臂环简易版程序 V0.0.1================================
 * 念通智能
 * 简述外设功能
 * 1.htim10:	周期2ms			中断优先级：1		作用：采集EMG
 * 2.htim11:	周期50ms		中断优先级：5		作用：更新RGB灯色状态
 * 3.htim3:		周期200ms		中断优先级：5		作用：循环控制BOOTH_LED
 * 4.htim9:		周期10ms		中断优先级：5		作用：判断当前各种工作状态
 *
 *
 *
 * EXIT_GPIO2:	周期10ms		中断优先级：2		作用：IMU中断引脚
 *
 *
 *
 *
 *
 * */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t AD_Value[20];	/* 8->EMG;1->battery*/
//==============================System work PV==================================
uint8_t work_state;				//工作状态变量	1：充电充满  2：正在充电	3：正常工作	  4：电量不足  5：电量微弱请及时充电
uint8_t Booth_state;			//蓝牙连接状态  0：未连接   1：连接状态
uint8_t Charge_state;			//充电状态		0：正在充   1：已经充满
uint8_t htim9_flag;				//定时器9旗标
uint8_t htim10_flag;			//定时器10旗标
uint8_t TimeFlag;				//EMG中断旗标
uint8_t IMUFlag;				//IMU中断旗标
uint8_t SystemFlag=0;			//完成初始化旗标

//================================IMU 数据测试函数====================================
short gyro[3], accel[3], sensors;
float Pitch,Roll,Yaw;
float q0,q1,q2,q3;
uint8_t OutPut_Buffer[11];


//===============串口1中断接受变量，4个字节用来判断华总发出的指令 ====================
uint8_t	uart_flag;
uint8_t RxBuffer[4];
uint8_t	IMU_state=0;				//是否开启IMU读取				0：关闭		1：开启
uint8_t	Motor_work_time;		//是否开启电机震动			0：关闭		else：开启*100ms的震动


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  WKUP_init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	HAL_ADC_Start_DMA(&hadc1,AD_Value,9);				//开启ADC转换


	Motor_Open();		//开电机
	Open_LED(0);		//开白色全灯
	HAL_Delay(300);		//稍作延时
	Motor_Close();		//关电机
	Li_battery = AD_Value[8];				//因为ADC采集Li_battery在主函数中执行，先提前采集一次数据
	HAL_Delay(50);		//稍作延时

	HAL_TIM_Base_Start_IT(&htim3);			//蓝牙闪烁htim 200ms/T
	HAL_TIM_Base_Start_IT(&htim9);			//状态判断htim	 90ms/T
	HAL_TIM_Base_Start_IT(&htim10);			//EMG采集htim	 2ms/T
	HAL_TIM_Base_Start_IT(&htim11);			//LED灯状态更新htim  50ms/T


  HAL_Delay(10);
  DMP_init();			//IMU初始化
  HAL_Delay(10);



  SystemFlag = 1;





  while (1)
  {
			HAL_UART_Receive_IT(&huart1,RxBuffer,sizeof(RxBuffer));		//串口1接收中断
//		if(HAL_UART_Receive(&huart1,RxBuffer,sizeof(RxBuffer),1000) == HAL_OK)
//		{
//				//定长解析
//				if((RxBuffer[0]==0xCC)&&(RxBuffer[1]==0xDD))
//				{
//					if(RxBuffer[2] == 0xFF)
//					{
//						if(RxBuffer[3] == 0x66)
//						{//开启IMU读取
//							IMU_state = 1;
//						}			
//						else if(RxBuffer[3] == 0x77)
//						{//关闭IMU读取
//							IMU_state = 0;
//						}
//					}
//					else if(RxBuffer[2] == 0xEE)
//					{//将time传递给电机震动部分
//						Motor_work_time = RxBuffer[3];
//					}			
//				}	
//		}
	
		if(Motor_work_time!=0)
		{//串口读取到数据需要开启电机，对应延时时间
			Motor_Open();
			HAL_Delay(Motor_work_time*100);
			Motor_Close();
			Motor_work_time = 0;		
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{//蓝牙指示灯工作定时器 200ms/T
		htim3_work_serve();
	}
	if(htim == &htim5)
	{//1MS

	}	
	if(htim == &htim9)
	{//判断工作状态 10ms/T
		htim9_work_serve();
	}
	if(htim == &htim10)
	{//采集EMG 2ms  同时因为这个最高优先级的中断,也担负IMU的串口数据发送
	  get_ADCdata();	//获取9路ADC数据
	  //get_ADCdata_Double_for_Onechannel();
		/* 发送16位 是1.5字节一个数据			发送18位是 2个字节一个数据*/
		
	  if(HAL_UART_Transmit(&huart1, Emg_Data_Sent, 16, 0xff)==HAL_OK)
		{TimeFlag = 0;}
	  else
		{Error_Handler();}
		if(IMU_state == 1)
		{//串口开启IMU状态
			if(IMUFlag == 1)
			{
				IMUFlag = 0;
				if(HAL_UART_Transmit(&huart1, Imu_Data_Sent, 8, 0xff)==HAL_OK)
					IMUFlag = 0;
				else
					Error_Handler();
			}
	  }
	}//end of tim10
	if(htim == &htim11)
	{//电池状态判断 50ms
		htim11_work_serve();
	}
}

/*****************中断接收命令，trigger功能，CommandRecevFlag*********************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{

				//定长解析
				if((RxBuffer[0]==0xCC)&&(RxBuffer[1]==0xDD))
				{
					if(RxBuffer[2] == 0xFF)
					{
						if(RxBuffer[3] == 0x66)
						{//开启IMU读取
							IMU_state = 1;
						}			
						else if(RxBuffer[3] == 0x77)
						{//关闭IMU读取
							IMU_state = 0;
						}
					}
					else if(RxBuffer[2] == 0xEE)
					{//将time传递给电机震动部分
						Motor_work_time = RxBuffer[3];
					}			
				}	
	//	HAL_UART_Receive_IT(&huart1,RxBuffer,sizeof(RxBuffer));		//串口1接收中断				
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {//错误中断内红灯不停闪烁
	  for(uint8_t i=0;i<20;i++)
	  {
		  Open_LED(3);
		  HAL_Delay(200);
		  Close_LED(3);
		  HAL_Delay(200);
	  }
	  PBout(5)=0;

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
