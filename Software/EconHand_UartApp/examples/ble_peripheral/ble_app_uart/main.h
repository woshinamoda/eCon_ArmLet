//**********************************************************
//****************ϵͳ��������ͷ�ļ�************************
//@notice������nrf52832ϵ�л���ifocus���س�����ֲ
//@ state��ֻ��Ҫ�޸ĺ궨����߾�����Ϳ�����ֲ������汾Ӳ��ifocus
#ifndef  _MAIN_H__
#define  _MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"

#include "driver_led.h"
#include "driver_power.h"
#include "driver_bat.h"
#include "driver_key.h"

//�������õ�ͷ�ļ�
#include "nrf_drv_saadc.h"			
#include "nrf_drv_gpiote.h"	

#include "nrf_ads1299.h"
#include "nrf_lsm6dsl.h"
#include "lsm6dsltr.h"

//DFU��Ҫ���õ�ͷ�ļ�
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "nrf_power.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"




/* USER CODE END Includes */



/* Private EconHand struct ----------------------------------------------------*/
/* USER CODE BEGIN struct*/

typedef struct
{
	bool			Key_Botton_state;					//��������״̬				true��������									false��δ������
	uint16_t	Key_BottonTime_count;			//�������¼�ʱ��
}Key_InitTypeDef;


typedef struct
{//RGB����ṹ��
	uint8_t		nrf_RGB_State;				//EconHand��Ӧ״̬		0������Ϩ��		1��δ����		2������		3�����			4������	
	uint16_t	RGB_count;						//RGB����˸������
	bool			APP_OrderFlag;				//APP�·�ָ���־λ		false��������˸��		true������ָ���
}RGB_InitTypeDef;

typedef struct
{//LSM6DSL����ṹ��
	short ax;								//x������ٶ�
	short ay;								//y������ٶ�
	short az;								//z������ٶ�
	short gx;								//x������ٶ�
	short gy;								//y������ٶ�
	short gz;								//z������ٶ�
	float acc_x;						//ʵ�� x ������ٶ� ��λmg
	float acc_y;						//ʵ�� y ������ٶ� ��λmg
	float acc_z;						//ʵ�� z ������ٶ� ��λmg
	float gyr_x;						//ʵ�� x ������ٶ� ��λdps
	float gyr_y;						//ʵ�� y ������ٶ� ��λdps
	float gyr_z;						//ʵ�� z ������ٶ� ��λdps	
	uint8_t	LSM6Dsl_ID;			//��̬��������ID
	uint8_t whoamI;
	uint8_t rst;
}IMU_InitTypeDef;

typedef struct
{//ADS1299����ṹ��
	int	boardStat;
	int	boardChannelDataInt[8]
}ECG_InitTypeDef;

typedef	struct
{//queue send data����ṹ��
	uint8_t DataCode_Both[138];				//���鷢��EEG+IMU����
	uint8_t	DataCode_ECG[126];				//��������EEG����
	uint8_t	DataCode_IMU[18];					//��������IMU����
	uint8_t	ECG_index;								//ECG�ɼ�������һ���ɼ�5����
	uint8_t	Data_number;							//��Ű����Զ�+1
	uint8_t	Mode;											//�ɼ�ģʽ				0��ֹͣ���ݲɼ�			1���ɼ�ECG+IMU����		2�����ɼ�ECG����		3�����ɼ�IMU����
	bool		IMU_GetOK;								//IMU�Ƿ�ɼ�
	bool		BleDat_En;								//���ݷ��ͷ���ʹ�����		ture��ʹ��CCCD				false��ʧ��CCCD
	bool		ble_Connect;							//��������״̬						ture��������					false��δ����
}SendData_InitTypeDef;


typedef	struct
{//��ص�����������ṹ��
	bool			first_collect;					//��һ�βɼ����ADC				
	uint8_t		m_adc_evt_counter;			//adcѭ���ɼ�����
	uint16_t	adc_dat_buf[10];				//adcֵ��������
	uint16_t	adc_dat_avr;						//adc��ƽ��ֵ
	uint16_t	bat_vol;								//����ʵ���ص�ѹֵmv
	
	uint8_t		last_precent;						//�ϴβ�����ص����ٷֱ�
	uint8_t		new_precent;						//���β�����ص����ٷֱ�
}BatCal_InitTypedef;





/* USER CODE END struct */




/* Private EconHand extern variable ------------------------------------------*/
/* USER CODE BEGIN extern*/
extern	Key_InitTypeDef				Key_UserDef;
extern	RGB_InitTypeDef				RGB_UserDef;
extern	IMU_InitTypeDef				IMU_UserDef;
extern	ECG_InitTypeDef				ECG_UserDef;
extern	SendData_InitTypeDef	SendData_UserDef;
extern	BatCal_InitTypedef		BatCal_UserDef;

extern	bool	Vcheck_state;	
/* USER CODE END  extern*/





/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//�豸��Ϣ��������
#define		company_name									"念通智能科技"							//URLת��"��ͨ���ܿƼ�"
#define		hardware_revision							"V0.0.1"
#define		software_revision							"V0.0.1"
#define		firmware_revision							"s132_nrf52_SDK_17.1.0"


#define APP_BLE_CONN_CFG_TAG            1                                          //BLE���ñ�ǩ
#define DEVICE_NAME                     "EconHand"                          		   //�豸����
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                 //UUID���� 128bit
#define APP_BLE_OBSERVER_PRIO           3                                          //Ӧ�ó����BLE�۲������ȼ�
#define APP_ADV_INTERVAL                64 	                  										 //�㲥��� 187.5ms  unit��0.625		range��20ms - 10.24sec 

#define	period_TIMER_INTERVAL						APP_TIMER_TICKS(10)												 //���ڶ�ʱ��ʱ����
#define	battery_TIMER_INTERVAL					APP_TIMER_TICKS(500)											 //��ض�ʱ��ʱ����


#define ADC_REF_VOLTAGE_IN_MILLIVOLTS   600      							//0.6V saadc�ڲ��ο�
#define ADC_PRE_SCALING_COMPENSATION    6    									// 1/6������
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS  0  										//����������ѹ����Ӳ��û�ж�������Ϊ0��
#define ADC_RES_10BIT  									1024									//10bit��saadc�ֱ���
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)
				
				
				

/* ������ģʽҪ����Ϊ��LE��ͨ����ģʽ�Ͳ�֧��BR/EDRģʽ	���������õ�timeout����ʵ��Ч��	 */
#define APP_ADV_DURATION                0                                    			 //�㲥��ʱ 10min			unit��10ms   60000
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)            //��С���Ӽ��
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)            //������Ӽ��
#define SLAVE_LATENCY                   0                                          
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)            
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                      //�״����Ӳ��������¼�
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                     //�´����������ʱ����
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                          //����Э�����Ӳ���ǰ�������Ĵ���
#define DEAD_BEEF                       0xDEADBEEF                                 //��ջת���������ֵ
#define UART_TX_BUF_SIZE                256                                        //����TX���ͻ����С
#define UART_RX_BUF_SIZE                256                                        //����RX���ջ����С

//LED GPIO	define
#define	LED_R     									    NRF_GPIO_PIN_MAP(0,27)
#define	LED_G     									    NRF_GPIO_PIN_MAP(0,26)
#define	LED_B     									    NRF_GPIO_PIN_MAP(0,25)

//Power GPIO define
#define DVDD_EN													NRF_GPIO_PIN_MAP(0,17)
#define AVDD_EN													NRF_GPIO_PIN_MAP(0,18)
#define	STA															NRF_GPIO_PIN_MAP(0,20)
#define	EXTI_STA												20




//ADS1299 GPIO define
#define  SPI_SS_PIN    								  NRF_GPIO_PIN_MAP(0,6)	
#define  SPI_SCK_PIN   								  NRF_GPIO_PIN_MAP(0,5)
#define  SPI_MISO_PIN								    NRF_GPIO_PIN_MAP(0,4)
#define  SPI_MOSI_PIN 								  NRF_GPIO_PIN_MAP(0,10)

#define	 ADS_DRDY										    NRF_GPIO_PIN_MAP(0,3)
#define	 ADS_START										  NRF_GPIO_PIN_MAP(0,7)
#define	 ADS_RESET										  NRF_GPIO_PIN_MAP(0,8)
#define	 ADS_PWDN										    NRF_GPIO_PIN_MAP(0,9)
#define	 EXTI_ECG												3

//LSM6DSL GPIO define
#define	 TWI_SCL_M											NRF_GPIO_PIN_MAP(0,30)
#define	 TWI_SDA_M											NRF_GPIO_PIN_MAP(0,29)
#define	 IMU_INT												NRF_GPIO_PIN_MAP(0,28)
#define	 EXTI_IMU												28

//key GPIO define
#define	 EXTI_KEY												21
#define	 PWOER_KEY_PIN								 	NRF_GPIO_PIN_MAP(0,21)

//Vcheck GPIO define
#define	 EXTI_Vcheck										19
#define  Vcheck_PIN											NRF_GPIO_PIN_MAP(0,19)	

/* USER CODE END PD */









/* Private function making by user--------------------------------------------*/
/* USER CODE BEGIN PFP */

void	sleep_mode_enter(void);									//����˯��ģʽ
void	Sleep_withOut_red(void);								//����˯��ģʽ��������ɫLEDָʾ
void	Observe_Close_BleSoftDevice(void);			//�������Ͽ����������£���Ϲرչ㲥
void	APP_Close_BleADV(void);									//�����Ͽ������͹㲥



/* USER CODE  END  PFP */




#ifdef __cplusplus
}
#endif

#endif

