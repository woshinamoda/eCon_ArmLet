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
/*============================����ۻ����װ���� V0.0.1================================
 * ��ͨ����
 * �������蹦��
 * 1.htim10:	����2ms		�ж����ȼ���3		���ã��ɼ�EMG
 * 2.htim11:	����50ms		�ж����ȼ���6		���ã���ص����жϼ�⣬����RGB��ɫ״̬
 * 3.htim3:		����200ms		�ж����ȼ���4		���ã�ѭ������BOOTH_LED
 * 4.htim9:		����10ms		�ж����ȼ���4		���ã��жϵ�ǰ���ֹ���״̬
 *
 * EXIT_GPIO2:	����10ms		�ж����ȼ���2		���ã�IMU�ж�����
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
uint8_t work_state;				//����״̬����	1��������  2�����ڳ��	3����������	  4����������  5������΢���뼰ʱ���
uint8_t Booth_state;			//��������״̬  0��δ����   1������״̬
uint8_t Charge_state;			//���״̬		0�����ڳ�   1���Ѿ�����
uint8_t htim9_flag;				//��ʱ��9���
uint8_t TimeFlag;				//EMG�ж����


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

//	Motor_Open();		//�����
//	Open_LED(0);		//����ɫ��
//	HAL_Delay(200);		//������ʱ
//	Motor_Close();		//�ص��
//	Close_LED(0);		//����ɫ��
//	Open_LED(2);		//������ɫLED����ʽ�������빤��״̬





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
	{//����ָʾ�ƹ�����ʱ�� 200ms/T
		switch(Booth_state)
		{
		case 0://�������û�����ӣ�ѭ����תIO��ʹ����˸
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
			break;
		case 1://���������������״̬��ֱ�Ӹߵ�ƽʹ�䳣��
			PBout(13) = 0;
			break;
		}
		if(PBin(4) == 1)
		{//ֻ���ڽ��������������
			switch(Charge_state)
			{
			case 0://���û�г�������ɫ��˸
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
				break;
			case 1://���������ɫ����
				PBout(6) = 0;
				break;
			}
		}
	}

	if(htim == &htim9)
	{//�жϹ���״̬ 10ms/T
		if(1)
		work_state = 3;		//����״̬������ɫ����---���ȼ����
		if(LI_Old_state == 1)
		work_state = 4;		//�еȵ�����ɫ����
		if(LI_Old_state == 0)
		work_state = 5;		//�͵ȵ�����ɫ����
		if(PBin(4) == 1)
		{//�����������״̬
			switch(PAin(12))
			{
			case 0:		//CHRG = 0 ������ڳ��
				work_state = 2;
				break;
			case 1:		//CHRG = 1 ��س���
				work_state = 1;
				break;
			}
		}//end of PB4 judege
		else
		{


		}
		if(PBin(12) == 1)
		{//������������״̬
			Booth_state = 1;
		}
		else
		{//�������ڶϿ�״̬
			Booth_state = 0;
		}//end of PB12 judge
	}//end of htim9 judge
	if(htim == &htim10)
	{
		TimeFlag = 1;
		get_ADCdata();	//��ȡ9·ADC����
	}
	if(htim == &htim11)
	{
		Bat_Filter();	//��ص����жϺ���
		switch(work_state)
		{
		case 1:
			Charge_state = 1;	//������
			Close_LED(2);
			Close_LED(3);		//������ʱ��������ɫ��ת���Ͳ�Ҫȫ���رգ��������
			break;
		case 2:
			Charge_state = 0;	//���ڳ��
			Close_LED(2);
			Close_LED(3);		//������ʱ��������ɫ��ת���Ͳ�Ҫȫ���رգ��������
			break;
		case 3:
			Close_LED(0);
			Open_LED(2);		//����״̬����ɫ
			break;
		case 4:
			Close_LED(0);
			Open_LED(4);		//�����еȿ��Ƶ�
			break;
		case 5:
			Close_LED(0);
			Open_LED(3);		//����΢���������
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
