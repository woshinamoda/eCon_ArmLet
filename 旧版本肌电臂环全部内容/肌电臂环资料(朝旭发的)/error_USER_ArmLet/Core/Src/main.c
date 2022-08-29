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
 * 1.htim10:	周期2ms		中断优先级：3		作用：采集EMG
 * 2.htim11:	周期50ms		中断优先级：6		作用：电池电量判断检测，更新RGB灯色状态
 * 3.htim3:		周期200ms		中断优先级：4		作用：循环控制BOOTH_LED
 * 4.htim9:		周期10ms		中断优先级：4		作用：判断当前各种工作状态
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
//==============================User ADC Test PV================================
uint32_t AD_Value[20];	/* 8->EMG;1->battery*/
uint32_t User_ADC[9]={0};
//==============================System work PV==================================
uint8_t work_state;				//工作状态变量	1：充电充满  2：正在充电	3：正常工作	  4：电量不足  5：电量微弱请及时充电
uint8_t Booth_state;			//蓝牙连接状态  0：未连接   1：连接状态
uint8_t Charge_state;			//充电状态		0：正在充   1：已经充满
uint8_t htim9_flag;				//定时器9旗标
uint8_t TimeFlag;				//EMG中短旗标


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
  //WKUP_init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_TIM3_Init();
  MX_DMA_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_ADC_Start_DMA(&hadc1,User_ADC,9);

//	Motor_Open();		//开电机
//	Open_LED(0);		//开紫色灯
//	HAL_Delay(200);		//稍作延时
//	Motor_Close();		//关电机
//	Close_LED(0);		//关紫色灯
//	Open_LED(2);		//单开蓝色LED，正式工作进入工作状态





//	HAL_TIM_Base_Start_IT(&htim3);
//	HAL_TIM_Base_Start_IT(&htim9);
//	HAL_TIM_Base_Start_IT(&htim10);
//	HAL_TIM_Base_Start_IT(&htim11);

  while (1)
  {

//	  HAL_Delay(500);
//	  HAL_ADC_Start(&hadc1);
//	  HAL_ADC_Start_DMA(&hadc1,AD_Value,9);
//	  HAL_ADC_Start(&hadc1);
//	  HAL_ADC_PollForConversion(&hadc1,100);
//	  AD_Value[0] = HAL_ADC_GetValue(&hadc1);
//	  HAL_ADC_Stop(&hadc1);







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
		switch(Booth_state)
		{
		case 0://如果蓝牙没有连接，循环反转IO口使其闪烁
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
			break;
		case 1://如果蓝牙处于连接状态，直接高低平使其常量
			PBout(13) = 0;
			break;
		}
		if(PBin(4) == 1)
		{//只有在接入充电器的情况下
			switch(Charge_state)
			{
			case 0://如果没有充满，绿色闪烁
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
				break;
			case 1://如果充满绿色常量
				PBout(6) = 0;
				break;
			}
		}
	}

	if(htim == &htim9)
	{//判断工作状态 10ms/T
		if(1)
		work_state = 3;		//正常状态下是蓝色常量---优先级最低
		if(LI_Old_state == 1)
		work_state = 4;		//中等电量黄色常量
		if(LI_Old_state == 0)
		work_state = 5;		//低等电量红色常量
		if(PBin(4) == 1)
		{//如果接入充电器状态
			switch(PAin(12))
			{
			case 0:		//CHRG = 0 电池正在充电
				work_state = 2;
				break;
			case 1:		//CHRG = 1 电池充满
				work_state = 1;
				break;
			}
		}//end of PB4 judege
		else
		{


		}
		if(PBin(12) == 1)
		{//蓝牙处于连接状态
			Booth_state = 1;
		}
		else
		{//蓝牙处于断开状态
			Booth_state = 0;
		}//end of PB12 judge
	}//end of htim9 judge
	if(htim == &htim10)
	{
		TimeFlag = 1;
		get_ADCdata();	//获取9路ADC数据
	}
	if(htim == &htim11)
	{
		Bat_Filter();	//电池电量判断函数
		switch(work_state)
		{
		case 1:
			Charge_state = 1;	//充满电
			Close_LED(2);
			Close_LED(3);		//其他定时器里用绿色反转，就不要全部关闭，以免干扰
			break;
		case 2:
			Charge_state = 0;	//正在充电
			Close_LED(2);
			Close_LED(3);		//其他定时器里用绿色反转，就不要全部关闭，以免干扰
			break;
		case 3:
			Close_LED(0);
			Open_LED(2);		//正常状态开蓝色
			break;
		case 4:
			Close_LED(0);
			Open_LED(4);		//电量中等开黄灯
			break;
		case 5:
			Close_LED(0);
			Open_LED(3);		//电量微弱，开红灯
			break;
		}
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
  {
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
