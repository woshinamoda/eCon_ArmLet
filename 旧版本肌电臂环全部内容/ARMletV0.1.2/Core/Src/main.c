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
 * 1.htim10:	����2ms			�ж����ȼ���1		���ã��ɼ�EMG
 * 2.htim11:	����50ms		�ж����ȼ���5		���ã�����RGB��ɫ״̬
 * 3.htim3:		����200ms		�ж����ȼ���5		���ã�ѭ������BOOTH_LED
 * 4.htim9:		����10ms		�ж����ȼ���5		���ã��жϵ�ǰ���ֹ���״̬
 *
 *
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
uint32_t AD_Value[20];	/* 8->EMG;1->battery*/
//==============================System work PV==================================
uint8_t work_state;				//����״̬����	1��������  2�����ڳ��	3����������	  4����������  5������΢���뼰ʱ���
uint8_t Booth_state;			//��������״̬  0��δ����   1������״̬
uint8_t Charge_state;			//���״̬		0�����ڳ�   1���Ѿ�����
uint8_t htim9_flag;				//��ʱ��9���
uint8_t htim10_flag;			//��ʱ��10���
uint8_t TimeFlag;				//EMG�ж����
uint8_t IMUFlag;				//IMU�ж����
uint8_t SystemFlag=0;			//��ɳ�ʼ�����

//================================IMU ���ݲ��Ժ���====================================
short gyro[3], accel[3], sensors;
float Pitch,Roll,Yaw;
float q0,q1,q2,q3;
uint8_t OutPut_Buffer[11];


//===============����1�жϽ��ܱ�����4���ֽ������жϻ��ܷ�����ָ�� ====================
uint8_t	uart_flag;
uint8_t RxBuffer[4];
uint8_t	IMU_state=0;				//�Ƿ���IMU��ȡ				0���ر�		1������
uint8_t	Motor_work_time;		//�Ƿ��������			0���ر�		else������*100ms����


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

	HAL_ADC_Start_DMA(&hadc1,AD_Value,9);				//����ADCת��


	Motor_Open();		//�����
	Open_LED(0);		//����ɫȫ��
	HAL_Delay(300);		//������ʱ
	Motor_Close();		//�ص��
	Li_battery = AD_Value[8];				//��ΪADC�ɼ�Li_battery����������ִ�У�����ǰ�ɼ�һ������
	HAL_Delay(50);		//������ʱ

	HAL_TIM_Base_Start_IT(&htim3);			//������˸htim 200ms/T
	HAL_TIM_Base_Start_IT(&htim9);			//״̬�ж�htim	 90ms/T
	HAL_TIM_Base_Start_IT(&htim10);			//EMG�ɼ�htim	 2ms/T
	HAL_TIM_Base_Start_IT(&htim11);			//LED��״̬����htim  50ms/T


  HAL_Delay(10);
  DMP_init();			//IMU��ʼ��
  HAL_Delay(10);



  SystemFlag = 1;





  while (1)
  {
			HAL_UART_Receive_IT(&huart1,RxBuffer,sizeof(RxBuffer));		//����1�����ж�
//		if(HAL_UART_Receive(&huart1,RxBuffer,sizeof(RxBuffer),1000) == HAL_OK)
//		{
//				//��������
//				if((RxBuffer[0]==0xCC)&&(RxBuffer[1]==0xDD))
//				{
//					if(RxBuffer[2] == 0xFF)
//					{
//						if(RxBuffer[3] == 0x66)
//						{//����IMU��ȡ
//							IMU_state = 1;
//						}			
//						else if(RxBuffer[3] == 0x77)
//						{//�ر�IMU��ȡ
//							IMU_state = 0;
//						}
//					}
//					else if(RxBuffer[2] == 0xEE)
//					{//��time���ݸ�����𶯲���
//						Motor_work_time = RxBuffer[3];
//					}			
//				}	
//		}
	
		if(Motor_work_time!=0)
		{//���ڶ�ȡ��������Ҫ�����������Ӧ��ʱʱ��
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
	{//����ָʾ�ƹ�����ʱ�� 200ms/T
		htim3_work_serve();
	}
	if(htim == &htim5)
	{//1MS

	}	
	if(htim == &htim9)
	{//�жϹ���״̬ 10ms/T
		htim9_work_serve();
	}
	if(htim == &htim10)
	{//�ɼ�EMG 2ms  ͬʱ��Ϊ���������ȼ����ж�,Ҳ����IMU�Ĵ������ݷ���
	  get_ADCdata();	//��ȡ9·ADC����
	  //get_ADCdata_Double_for_Onechannel();
		/* ����16λ ��1.5�ֽ�һ������			����18λ�� 2���ֽ�һ������*/
		
	  if(HAL_UART_Transmit(&huart1, Emg_Data_Sent, 16, 0xff)==HAL_OK)
		{TimeFlag = 0;}
	  else
		{Error_Handler();}
		if(IMU_state == 1)
		{//���ڿ���IMU״̬
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
	{//���״̬�ж� 50ms
		htim11_work_serve();
	}
}

/*****************�жϽ������trigger���ܣ�CommandRecevFlag*********************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{

				//��������
				if((RxBuffer[0]==0xCC)&&(RxBuffer[1]==0xDD))
				{
					if(RxBuffer[2] == 0xFF)
					{
						if(RxBuffer[3] == 0x66)
						{//����IMU��ȡ
							IMU_state = 1;
						}			
						else if(RxBuffer[3] == 0x77)
						{//�ر�IMU��ȡ
							IMU_state = 0;
						}
					}
					else if(RxBuffer[2] == 0xEE)
					{//��time���ݸ�����𶯲���
						Motor_work_time = RxBuffer[3];
					}			
				}	
	//	HAL_UART_Receive_IT(&huart1,RxBuffer,sizeof(RxBuffer));		//����1�����ж�				
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
  {//�����ж��ں�Ʋ�ͣ��˸
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
