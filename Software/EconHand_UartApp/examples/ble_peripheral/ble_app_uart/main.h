//**********************************************************
//****************系统声明集中头文件************************
//@notice：方便nrf52832系列基于ifocus板载程序移植
//@ state：只需要修改宏定义或者句柄，就可以移植至任意版本硬件ifocus
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

//外设引用的头文件
#include "nrf_drv_saadc.h"			
#include "nrf_drv_gpiote.h"	

#include "nrf_ads1299.h"
#include "nrf_lsm6dsl.h"
#include "lsm6dsltr.h"

//DFU需要引用的头文件
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
	bool			Key_Botton_state;					//按键触发状态				true：被触发									false：未被触发
	uint16_t	Key_BottonTime_count;			//按键按下计时器
}Key_InitTypeDef;


typedef struct
{//RGB句柄结构体
	uint8_t		nrf_RGB_State;				//EconHand对应状态		0：待机熄灭		1：未连接		2：连接		3：充电			4：充满	
	uint16_t	RGB_count;						//RGB灯闪烁计数器
	bool			APP_OrderFlag;				//APP下发指令标志位		false：正常闪烁灯		true：接收指令灯
}RGB_InitTypeDef;

typedef struct
{//LSM6DSL句柄结构体
	short ax;								//x方向加速度
	short ay;								//y方向加速度
	short az;								//z方向加速度
	short gx;								//x方向角速度
	short gy;								//y方向角速度
	short gz;								//z方向角速度
	float acc_x;						//实际 x 方向加速度 单位mg
	float acc_y;						//实际 y 方向加速度 单位mg
	float acc_z;						//实际 z 方向加速度 单位mg
	float gyr_x;						//实际 x 方向角速度 单位dps
	float gyr_y;						//实际 y 方向角速度 单位dps
	float gyr_z;						//实际 z 方向角速度 单位dps	
	uint8_t	LSM6Dsl_ID;			//姿态传感器的ID
	uint8_t whoamI;
	uint8_t rst;
}IMU_InitTypeDef;

typedef struct
{//ADS1299句柄结构体
	int	boardStat;
	int	boardChannelDataInt[8]
}ECG_InitTypeDef;

typedef	struct
{//queue send data句柄结构体
	uint8_t DataCode_Both[138];				//数组发送EEG+IMU数据
	uint8_t	DataCode_ECG[126];				//单独发送EEG数据
	uint8_t	DataCode_IMU[18];					//单独发送IMU数据
	uint8_t	ECG_index;								//ECG采集点数：一包采集5个点
	uint8_t	Data_number;							//序号包，自动+1
	uint8_t	Mode;											//采集模式				0：停止数据采集			1：采集ECG+IMU数据		2：仅采集ECG数据		3：仅采集IMU数据
	bool		IMU_GetOK;								//IMU是否采集
	bool		BleDat_En;								//数据发送服务使能旗标		ture：使能CCCD				false：失能CCCD
	bool		ble_Connect;							//蓝牙连接状态						ture：已连接					false：未连接
}SendData_InitTypeDef;


typedef	struct
{//电池电量测量句柄结构体
	bool			first_collect;					//第一次采集电池ADC				
	uint8_t		m_adc_evt_counter;			//adc循环采集次数
	uint16_t	adc_dat_buf[10];				//adc值缓存数组
	uint16_t	adc_dat_avr;						//adc的平均值
	uint16_t	bat_vol;								//计算实测电池电压值mv
	
	uint8_t		last_precent;						//上次测量电池电量百分比
	uint8_t		new_precent;						//本次测量电池电量百分比
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
//设备信息服务内容
#define		company_name									"蹇甸�氭櫤鑳界鎶�"							//URL转换"念通智能科技"
#define		hardware_revision							"V0.0.1"
#define		software_revision							"V0.0.1"
#define		firmware_revision							"s132_nrf52_SDK_17.1.0"


#define APP_BLE_CONN_CFG_TAG            1                                          //BLE配置标签
#define DEVICE_NAME                     "EconHand"                          		   //设备名称
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                 //UUID类型 128bit
#define APP_BLE_OBSERVER_PRIO           3                                          //应用程序的BLE观察者优先级
#define APP_ADV_INTERVAL                64 	                  										 //广播间隔 187.5ms  unit：0.625		range：20ms - 10.24sec 

#define	period_TIMER_INTERVAL						APP_TIMER_TICKS(10)												 //周期定时器时间间隔
#define	battery_TIMER_INTERVAL					APP_TIMER_TICKS(500)											 //电池定时器时间间隔


#define ADC_REF_VOLTAGE_IN_MILLIVOLTS   600      							//0.6V saadc内部参考
#define ADC_PRE_SCALING_COMPENSATION    6    									// 1/6的增益
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS  0  										//二极管正向压降（硬件没有二极管视为0）
#define ADC_RES_10BIT  									1024									//10bit的saadc分辨率
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)
				
				
				

/* ！蓝牙模式要设置为：LE普通发现模式和不支持BR/EDR模式	，这样设置的timeout才有实际效果	 */
#define APP_ADV_DURATION                0                                    			 //广播超时 10min			unit：10ms   60000
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)            //最小连接间隔
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)            //最大连接间隔
#define SLAVE_LATENCY                   0                                          
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)            
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                      //首次连接参数更新事件
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                     //下次连接申请的时间间隔
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                          //放弃协商连接参数前尝试最大的次数
#define DEAD_BEEF                       0xDEADBEEF                                 //堆栈转储错误代码值
#define UART_TX_BUF_SIZE                256                                        //串口TX发送缓存大小
#define UART_RX_BUF_SIZE                256                                        //串口RX接收缓存大小

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

void	sleep_mode_enter(void);									//进入睡眠模式
void	Sleep_withOut_red(void);								//进入睡眠模式，不带红色LED指示
void	Observe_Close_BleSoftDevice(void);			//在主动断开蓝牙条件下，配合关闭广播
void	APP_Close_BleADV(void);									//主动断开蓝牙和广播



/* USER CODE  END  PFP */




#ifdef __cplusplus
}
#endif

#endif

