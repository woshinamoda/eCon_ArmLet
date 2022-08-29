
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"

#include "ble_bas.h"			//电池服务头文件
#include "my_ble_dis.h"		//设备信息服务头文件
#include "nrf_queue.h"		//队列头文件

/* USER CODE END Includes */


/* Private variable & struct -------------------------------------------------*/
/* USER CODE BEGIN V&S */
Key_InitTypeDef				Key_UserDef = {
																		false,										//按键未被触发
																		0,												//计数器清零
																		};

RGB_InitTypeDef				RGB_UserDef = {
																		 1,												//EconHand对应状态		0：待机熄灭		1：未连接		2：连接		3：充电			4：充满	
																		 0,												//RGB灯闪烁计数器
																		 false,										//APP下发指令标志位		false：正常闪烁灯		true：接收指令灯
																		};

IMU_InitTypeDef				IMU_UserDef = {0};

ECG_InitTypeDef				ECG_UserDef = {
																		 0,												//boardStat
																		{0},											//boardChannelDataInt[8]
																		};

SendData_InitTypeDef	SendData_UserDef = {
																						{0xFA,0xFA,},			//ECG + IMU 数组
																						{0xFA,0xFA,},			//ECG数组
																						{0xFA,0xFA,},			//IMU数组
																						0,								//ECG采集点数 0 - 4
																						0,								//序号包：自动+1
																						2,								//mode 
																						false,						//IMU采集状态
																						false,						//关闭串口透传CCCD
																						false,						//蓝牙连接状态
																					};

BatCal_InitTypedef		BatCal_UserDef = {
																					true,								//第一次采集电池adc
																					0,									//adc循环采集次数
																					{0},								//adc值缓存数组
																					0,									//adc的平均值
																					0,									//电池电压mv
																					0,									//上次电池电量百分比
																					0,									//本次电池电量百分比
};
/* USER CODE BEGIN V&S */






/* Private user instance ------------------------------------------------------*/
/* USER CODE BEGIN 0 */
ble_nus_t	m_dat;																					
BLE_NUS_DEF(m_dat, NRF_SDH_BLE_TOTAL_LINK_COUNT);												//BLE NUS service instance
NRF_BLE_GATT_DEF(m_gatt);                                               //新增gatt实例                                                      
NRF_BLE_QWR_DEF(m_qwr);    																							//新增qwr实例                                                          
BLE_ADVERTISING_DEF(m_advertising);  																		//新增广播实例    
BLE_BAS_DEF(m_bas);																											//新增电池服务实例
APP_TIMER_DEF(m_app_timer_id);																					//新增周期定时器实例
APP_TIMER_DEF(m_bat_timer_id);																					//新增电池定时器实例																
/* USER CODE END 0 */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;//传输端口最大数据长度
static ble_uuid_t m_adv_uuids[]          =                 							//通用唯一服务 标识符                      
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};
/* USER CODE END 0 */

/* Private user variables ----------------------------------------------------*/
/* USER CODE BEGIN PV */
bool		isNeedDisconnect = false;																					//用户主动断开蓝牙				0:不断开										1：用户主动要求断开
bool		adv_started = false;																							//广播是否连接旗标				0：断开连接，广播工作				1：连接蓝牙，广播停止
bool		Vcheck_state = false;																							//接入充电器状态					true：确实接入充电器				false：没有接入充电器
static	nrf_saadc_value_t	saadc_val;																			//ADC存放电池电量采集数值变量
/* USER CODE  END  PV */



/* Private qeue send Data prepare  -------------------------------------------*/
/* USER CODE BEGIN PDP */
typedef struct 																							
{
	uint8_t * p_data;																											//实际存放数据的数组								
	uint16_t length;																											//这组数据长度
} buffer_t;


NRF_QUEUE_DEF(buffer_t, m_queue, 30, NRF_QUEUE_MODE_NO_OVERFLOW);				//建立循环队列
uint8_t			m_data_array[4200];																					//实际数据缓存数组
uint16_t		m_data_count;																								//缓存数组位置

buffer_t		m_Dat_buf;
void ble_data_send_queue(void)
{//数据发送队列服务函数
	uint32_t	err_code;
	uint16_t	length=0;
	static bool retry = false;
	if(retry)	//如果发送旗标置1
	{
		length = m_Dat_buf.length;
		err_code = ble_nus_data_send(&m_dat, m_Dat_buf.p_data, &length, m_conn_handle);
		//NRF_LOG_INFO("send data err_code:%d", err_code);
		if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_RESOURCES) &&
			 (err_code != NRF_ERROR_NOT_FOUND))// && (err_code != BLE_ERROR_INVALID_CONN_HANDLE))
		{
			APP_ERROR_CHECK(err_code);
		}
		if(err_code == NRF_SUCCESS)
		{
			retry = false;
		}
	}
	while(!nrf_queue_is_empty(&m_queue) && !retry)
		{//1.队列非空	2.发送旗标为0
			err_code = nrf_queue_pop(&m_queue, &m_Dat_buf);
			if(err_code == NRF_ERROR_NOT_FOUND)
			{//有可能点击太快，已经出栈以后再次进入？？？
				break;
			}
			APP_ERROR_CHECK(err_code);
			length = m_Dat_buf.length;
			err_code = ble_nus_data_send(&m_dat, m_Dat_buf.p_data, &length, m_conn_handle);
			if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_RESOURCES) &&
				 (err_code != NRF_ERROR_NOT_FOUND))// && (err_code != BLE_ERROR_INVALID_CONN_HANDLE))
			{
				APP_ERROR_CHECK(err_code);
			}
			if(err_code == NRF_SUCCESS)
			{
				retry = false;
			}
			else
			{//如果发送失败：旗标置1，在上面判断收到ACK后发送
				retry = true;
				break;
			}
	}
}

/* USER CODE END PDP */



/* Private timer period function------------------------------------------------*/
/* USER CODE BEGIN PFP */

static void adc_configure(void)
{//配置ADC初始化
	ret_code_t err_code;

	nrf_saadc_channel_config_t config =
	NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);		//AIN0 对应 P0.02口
	
	//初始化saadc
	err_code = nrf_drv_saadc_init(NULL, NULL);												//saadc的初始化
	APP_ERROR_CHECK(err_code);

	//加入通道配置
	err_code = nrf_drv_saadc_channel_init(0, &config);
	APP_ERROR_CHECK(err_code);
}

static void Nordic_battery_timeout_handle(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	ret_code_t	err_code;
		
	adc_configure();																																	//初始化一次saadc
	nrf_drv_saadc_sample_convert(0,&saadc_val);																				//启动一次ADC采样
	BatCal_UserDef.adc_dat_buf[BatCal_UserDef.m_adc_evt_counter] = saadc_val;					
	BatCal_UserDef.m_adc_evt_counter++;
	if(BatCal_UserDef.m_adc_evt_counter == 10)
	{
		BatCal_UserDef.m_adc_evt_counter = 0;
		BatdataBubbleSort(BatCal_UserDef.adc_dat_buf , 10);															//冒泡排序，数据从小到大排列
		BatCal_UserDef.adc_dat_avr = GetBatAvr(BatCal_UserDef.adc_dat_buf , 10);				//求排序完后ADC的平均值
		
		BatCal_UserDef.bat_vol = ADC_RESULT_IN_MILLI_VOLTS(BatCal_UserDef.adc_dat_avr);	//求得电池电压
		NRF_LOG_INFO("batter_adc_data: %d", BatCal_UserDef.bat_vol);		
	
		if(BatCal_UserDef.first_collect == true)
		{//防止第一次在抖动空间没有读出数据
			BatCal_UserDef.new_precent = First_battery_level(BatCal_UserDef.bat_vol);			//无视抖动空间的计算电量百分比
			BatCal_UserDef.last_precent = BatCal_UserDef.new_precent;
			BatCal_UserDef.first_collect = false;		
		}
		else
		{//后续读取数据，新值传递旧值。在抖动空间直接替代
			BatCal_UserDef.new_precent = User_Set_battery_level_10per(BatCal_UserDef.bat_vol,BatCal_UserDef.last_precent);		//求得电池电量百分比
			BatCal_UserDef.last_precent = BatCal_UserDef.new_precent;																													//数据存交给上次电池电量百分比
		}
		err_code = ble_bas_battery_level_update(&m_bas, BatCal_UserDef.new_precent, BLE_CONN_HANDLE_ALL);		//电池数据上传给主机
		if ((err_code != NRF_SUCCESS) &&
				(err_code != NRF_ERROR_INVALID_STATE) &&
				(err_code != NRF_ERROR_RESOURCES) &&
				(err_code != NRF_ERROR_BUSY) &&
				(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
			 )
		{
				NRF_LOG_INFO("Battery level Push OK!: %d", BatCal_UserDef.new_precent);
				APP_ERROR_HANDLER(err_code);
		}	
	}
	nrfx_saadc_uninit();	
}

void battery_timers_start(void)
{
	uint32_t	err_code;
	
	err_code =	app_timer_start(m_bat_timer_id, battery_TIMER_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}

void battery_timer_stop(void)
{
	uint32_t	err_code;
	
	err_code = app_timer_stop(m_bat_timer_id);
	APP_ERROR_CHECK(err_code);
}
/* USER CODE END PFP */





/* Private timer period function------------------------------------------------*/
/* USER CODE BEGIN PFP */
static void Nordic_period_timeout_handle(void * p_context)
{//（周期tick定时器）事件回调
	UNUSED_PARAMETER(p_context);
	
	//工作指示灯判断函数
	if(RGB_UserDef.APP_OrderFlag)
		APP_Order_Blink();
	else
		LED_Indicate_Mode();
	
	//按键长短按判断函数(非充电情况下有效)
	if((RGB_UserDef.nrf_RGB_State != 3)&&(RGB_UserDef.nrf_RGB_State !=4))
	{Key_Time_JudgeOnOff();}
	
}
void period_timers_start(void)
{	//period tick : 10ms
	uint32_t	err_code;
	err_code =	app_timer_start(m_app_timer_id, period_TIMER_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}
void period_timer_stop(void)
{
	uint32_t	err_code;
	err_code = app_timer_stop(m_app_timer_id);
	APP_ERROR_CHECK(err_code);
}
/* USER CODE END PFP */



/* Private service Battery function--------------------------------------------*/
/* USER CODE BEGIN PFP */
static void on_bas_evt(ble_bas_t * p_bas, ble_bas_evt_t * p_evt)
{//电池服务事件回调函数

	ret_code_t err_code;
    switch (p_evt->evt_type)
    {
        case BLE_BAS_EVT_NOTIFICATION_ENABLED:
							//battery_timers_start();
						 	NRF_LOG_INFO("使能了电池服务notify");
							
            break; 

        case BLE_BAS_EVT_NOTIFICATION_DISABLED:
							//battery_timer_stop();
							NRF_LOG_INFO("关闭了电池服务notify");
						
            break; 

        default:
            // No implementation needed.
            break;
    }	
}
static void bas_init(void)
{//电池服务初始化
	ret_code_t				err_code;	
	ble_bas_init_t		bas_init_obj;		//声明电池服务初始化结构体 bas_init_obj
	//清空电池服务初始化结构体
	memset(&bas_init_obj, 0, sizeof(bas_init_obj));
	
	//配置电池服务初始化结构体
	bas_init_obj.evt_handler = on_bas_evt;						//电池回调处理函数
	bas_init_obj.support_notification = true;					//空中规范：使能通知
	bas_init_obj.p_report_ref = NULL;									//无报告
	bas_init_obj.initial_batt_level = 100;						//电池分级
	
	bas_init_obj.bl_rd_sec        = SEC_OPEN;					//读开放
	bas_init_obj.bl_cccd_wr_sec   = SEC_OPEN;					//ccdd写开放
	bas_init_obj.bl_report_rd_sec = SEC_OPEN;					//读开放
	
	err_code = ble_bas_init(&m_bas, &bas_init_obj);
	APP_ERROR_CHECK(err_code);
}

static void EconHand_dis_init(void)
{//用户自定义设备信息服务初始化
	ret_code_t							err_code;
	ble_dis_init_t					dis_init_obj;
	memset(&dis_init_obj, 0, sizeof(dis_init_obj));
	
	ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, (char *)company_name);
	ble_srv_ascii_to_utf8(&dis_init_obj.hw_rev_str, (char *)hardware_revision);
	ble_srv_ascii_to_utf8(&dis_init_obj.sw_rev_str, (char *)software_revision);
	ble_srv_ascii_to_utf8(&dis_init_obj.fw_rev_str, (char *)firmware_revision);	
	
  dis_init_obj.dis_char_rd_sec =SEC_OPEN;

	err_code = ble_dis_init(&dis_init_obj);
  APP_ERROR_CHECK(err_code);
}




/* USER CODE END PFP */



/* Private service DFU function--------------------------------------------*/
/* USER CODE BEGIN PFP */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{//关机准备处理程序。在关闭过程中，将以1秒的间隔调用此函数，直到函数返回true。当函数返回true时，表示应用程序已准备好复位为DFU模式
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}
//注册优先级为0的应用程序关闭处理程序
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);
static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{//SoftDevice状态监视者事件处理函数
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
			  //表明Softdevice在复位之前已经禁用，告之bootloader启动时应跳过CRC
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //进入system off模式
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{//注册SoftDevice状态监视者，用于SoftDevice状态改变或者即将改变时接收SoftDevice事件
    .handler = buttonless_dfu_sdh_state_observer,
};
static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);
    //断开当前连接
    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}
static void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}

static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{	
	switch(event)
	{
		//================准备进入bootloader事件
		case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:	
		{
			//阻止设备在断开连接时广播
			ble_adv_modes_config_t config;
			advertising_config_get(&config);
			//连接断开后设备不自动进行广播
			config.ble_adv_on_disconnect_disabled = true;
			//修改广播配置
			ble_advertising_modes_config_set(&m_advertising, &config);
			//断开当前已经连接的所有其他绑定设备。在设备固件更新成功（或中止）后，需要在启动时接收服务更改指示
			uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
			break;		
		}
		//================函数返回后设备进入bootloader事件
		case BLE_DFU_EVT_BOOTLOADER_ENTER:
			break;
		//================进入bootloader失败事件
		case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
			break;
		//================发送响应失败事件
		case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
			break;
		//其他
		default:
			break;
	}
}
static void dfu_init()
{
	ret_code_t	err_code;

	//定义DFU服务初始化结构体
	ble_dfu_buttonless_init_t	dfus_init= {0};

	//设置DFU事件回调函数
	dfus_init.evt_handler = ble_dfu_evt_handler;

	//初始化DFU服务
	err_code = ble_dfu_buttonless_init(&dfus_init);

	//检查函数返回的错误代码
	APP_ERROR_CHECK(err_code);
}
/* USER CODE END PFP */



/* nordic GPIOTE User set function--------------------------------------------*/
/* USER CODE BEGIN PFP */
void Nordic_GPIOTE_Handle(nrf_drv_gpiote_pin_t	pin, nrf_gpiote_polarity_t action)
{//外设GPIOTE事件回调函数
	ret_code_t	err_send1, err_send2;
	uint16_t		CRC16=0;
	buffer_t		buf;
	if(pin == EXTI_ECG)
	{
		if((SendData_UserDef.BleDat_En)&&(SendData_UserDef.ble_Connect))//&&(test_order))
		{//建立连接，同时使能CCCD状态下
			if(SendData_UserDef.Mode == 0)
			{//不发送数据指令	
			
			}
			else if(SendData_UserDef.Mode == 1)
			{//发送数据指令 采集IMU + ECG
				if(m_data_count > 3800)
					m_data_count = 0;
				ADS1299_DataCut_InLine();									//读取ECG数据，将其存入SendData_UserDef.DataCode_ECG数组中
				SendData_UserDef.ECG_index++;
				if(SendData_UserDef.ECG_index == 5)
				{//数据采集完成
					SendData_UserDef.ECG_index = 0;					//采集够了5个点的8通道数据
					SendData_UserDef.Data_number++;					//序号包自动加1
					if(SendData_UserDef.IMU_GetOK == true)
					{//如果采集到了IMU数据
						SendData_UserDef.IMU_GetOK = false;		//旗标复位
						for(uint8_t j=4;j<124;j++)
						{//ECG数组数据转移到ECG+IMU数组上
							SendData_UserDef.DataCode_Both[j] = SendData_UserDef.DataCode_ECG[j];		//将ECG数据，将其存入SendData_UserDef.DataCode_Both数组中
						}
						LSM6DSL_DataCut_InLine();																									//将IMU数据，将其存入SendData_UserDef.DataCode_Both数组中
						SendData_UserDef.DataCode_Both[2] = 138;																	//带IMU数据长度
						SendData_UserDef.DataCode_Both[135] = CRC16 & 0xff;
						SendData_UserDef.DataCode_Both[136] =	CRC16>>8 & 0xff;
						SendData_UserDef.DataCode_Both[137] = SendData_UserDef.Data_number;				//序号包自动加1				
						/* 至此数据存入数组完成 */
						for(uint8_t i=0; i<138; i++)
						{
							m_data_array[m_data_count] = SendData_UserDef.DataCode_Both[i];					//ECG + IMU数据存入缓存数组m_data_array中
							m_data_count++;
						}
						buf.p_data = &m_data_array[m_data_count - 138];														//队列结构体头，插入缓存数组地址
						buf.length = SendData_UserDef.DataCode_Both[2];														//队列结构体长度，标注数组内对应长度
						err_send1  = nrf_queue_push(&m_queue, &buf);															//数据压入队列m_queue中

						ble_data_send_queue();
					}
					else
					{//如果没有采集到IMU数据
						SendData_UserDef.DataCode_ECG[2] = 126;																		//不带IMU数据长度
						SendData_UserDef.DataCode_ECG[123] = CRC16 & 0xff;
						SendData_UserDef.DataCode_ECG[124] = CRC16>>8 & 0xff;
						SendData_UserDef.DataCode_ECG[125] = SendData_UserDef.Data_number;				//序号包自动加1
						/* 至此数据存入数组完成 */
						for(uint8_t i=0; i<126; i++)
						{
							m_data_array[m_data_count] = SendData_UserDef.DataCode_ECG[i];					//ECG数据存入缓存数组m_data_array中
							m_data_count++;
						}
						buf.p_data = &m_data_array[m_data_count - 126];														//队列头地址
						buf.length = SendData_UserDef.DataCode_ECG[2];														//队列数据长度
						err_send2  = nrf_queue_push(&m_queue, &buf);															//数据压入队列m_queue中		

						ble_data_send_queue();
						
					}
				}
			}		
			else if(SendData_UserDef.Mode == 2)
			{//发送数据指令 只有数据EEG
				if(m_data_count > 3800)
					m_data_count = 0;
				ADS1299_DataCut_InLine();				//读取ECG数据，将其存入SendData_UserDef.DataCode_ECG[]数组中
				SendData_UserDef.ECG_index++;
				if(SendData_UserDef.ECG_index == 5)
				{//数据采集完成
					SendData_UserDef.ECG_index = 0;																						//采集够了5个点的8通道数据
					SendData_UserDef.Data_number++;																						//序号包自动加1			
					SendData_UserDef.DataCode_ECG[2] = 126;																		//只有ECG数据的长度
					SendData_UserDef.DataCode_ECG[123] = CRC16 & 0xff;												
					SendData_UserDef.DataCode_ECG[124] = CRC16>>8 & 0xff;					
					SendData_UserDef.DataCode_ECG[125] = SendData_UserDef.Data_number;						
					/* 至此数据存入数组完成 */
					for(uint8_t i=0; i<126; i++)
					{
						m_data_array[m_data_count] = SendData_UserDef.DataCode_ECG[i];					//ECG数据存入缓存数组m_data_array中
						m_data_count++;
					}
					buf.p_data = &m_data_array[m_data_count - 126];														//队列头地址
					buf.length = SendData_UserDef.DataCode_ECG[2];														//队列数据长度
					err_send2  = nrf_queue_push(&m_queue, &buf);															//数据压入队列m_queue中		

					ble_data_send_queue();			
				}
			}
			else if(SendData_UserDef.Mode == 3)
			{//发送数据指令 只有数据IMU

			}
		}
	}
	if(pin == EXTI_IMU)
	{
		ret_code_t	err_send3;
		uint16_t		CRC16 = 0;
		buffer_t		buf;		
		//NRF_LOG_INFO("IMU EXTI Trigger");
		if((SendData_UserDef.BleDat_En)&&(SendData_UserDef.ble_Connect))//&&(test_order))
		{//建立连接，同时使能CCCD状态下
			if(SendData_UserDef.Mode == 1)
			{//采集IMU + EEG数据
				Get_LSM6D_Data();		
				SendData_UserDef.IMU_GetOK = true;
			}
			else if(SendData_UserDef.Mode == 3)
			{//采集IMU数据
				if(m_data_count > 3800)
				m_data_count = 0;
				Get_LSM6D_Data();									//读取IMU数据
				SendData_UserDef.Data_number++;		//序号自动+1
				LSM6DSL_DataCut_IMU();						//IMU数据切入到数组中
				SendData_UserDef.DataCode_IMU[2] = 0x12;	//(18)
				SendData_UserDef.DataCode_IMU[15] = (uint8_t)CRC16 & 0xff;
				SendData_UserDef.DataCode_IMU[16] = (uint8_t)CRC16>>8 & 0xff;
				SendData_UserDef.DataCode_IMU[17] = SendData_UserDef.Data_number;
				for(uint8_t i=0; i<18; i++)
				{
					m_data_array[m_data_count] = SendData_UserDef.DataCode_IMU[i];					//IMU数据存入缓存数组m_data_array中
					m_data_count++;
				}
				buf.p_data = &m_data_array[m_data_count - 18];														//队列头地址
				buf.length = SendData_UserDef.DataCode_IMU[2];														//队列数据长度
				err_send3  = nrf_queue_push(&m_queue, &buf);															//数据压入队列m_queue中		
									
				ble_data_send_queue();
			}
		}
	}	
	if(pin == EXTI_KEY)
	{
		Key_Botton_Handle();		
	}	
	if(pin == EXTI_Vcheck)
	{
		Vcheck_Handle();
	}	
}

void EconHand_GPIOTE_init()
{//外设GPIOTE初始化
	ret_code_t	err_code;
	
	err_code = nrf_drv_gpiote_init();
	APP_ERROR_CHECK(err_code);
	
	//（false）对应低精度，功耗更低
	nrf_drv_gpiote_in_config_t EXTI_ECG_GPIO = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(false);		//ADS1299 下降沿开始读取数据	
	nrf_drv_gpiote_in_config_t EXTI_IMU_GPIO = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);		//LSM6DL	手册P23说明上升沿读取数据		
	nrf_drv_gpiote_in_config_t EXTI_KEY_GPIO = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);		//按键有上拉，检测开和关，双边沿检测			
	nrf_drv_gpiote_in_config_t EXTI_VCK_GPIO = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);		//充电器要检测插入和拔下，双边沿检测		

	EXTI_ECG_GPIO.pull = NRF_GPIO_PIN_NOPULL;	
	EXTI_IMU_GPIO.pull = NRF_GPIO_PIN_NOPULL;	
	EXTI_KEY_GPIO.pull = NRF_GPIO_PIN_NOPULL;	
	EXTI_VCK_GPIO.pull = NRF_GPIO_PIN_NOPULL;

	err_code = nrf_drv_gpiote_in_init(EXTI_ECG,&EXTI_ECG_GPIO,Nordic_GPIOTE_Handle);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_gpiote_in_init(EXTI_IMU,&EXTI_IMU_GPIO,Nordic_GPIOTE_Handle);
	APP_ERROR_CHECK(err_code);	
	
	err_code = nrf_drv_gpiote_in_init(EXTI_KEY,&EXTI_KEY_GPIO,Nordic_GPIOTE_Handle);
	APP_ERROR_CHECK(err_code);	
	
	err_code = nrf_drv_gpiote_in_init(EXTI_Vcheck,&EXTI_VCK_GPIO,Nordic_GPIOTE_Handle);
	APP_ERROR_CHECK(err_code);		
	
	nrf_drv_gpiote_in_event_enable(EXTI_ECG,true);	
	nrf_drv_gpiote_in_event_enable(EXTI_IMU,true);			
	nrf_drv_gpiote_in_event_enable(EXTI_KEY,true);	
	nrf_drv_gpiote_in_event_enable(EXTI_Vcheck,true);		
}

/* USER CODE END PFP */



/* nordic service Handle deal by ----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void nus_data_handler(ble_nus_evt_t * p_evt)
{//串口服务事件回调函数
		ret_code_t err_code;
		uint8_t 	test_code[5];
		uint16_t	length = 5;
		if(p_evt->type == BLE_NUS_EVT_TX_RDY)	
		{//如果发送事件就绪,主机下发ACK
			ble_data_send_queue();
		}
    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {//如果接收到APP下行指令
			uint16_t	length;
			uint8_t		data;
			length = p_evt->params.rx_data.length;
			data	 = p_evt->params.rx_data.p_data[0];
			if(length == 1)
			{
				RGB_UserDef.APP_OrderFlag = true;
				RGB_UserDef.RGB_count = 0;
				test_code[0] = data;
				err_code = ble_nus_data_send(&m_dat, test_code, &length, m_conn_handle);
				switch(data)
				{
					case 0x01:
						SendData_UserDef.Mode = 1;
						//test_order = 1;
						break;
					case 0x02:
						SendData_UserDef.Mode = 2;
						//test_order = 0;
						break;
					case 0x03:
						SendData_UserDef.Mode = 3;
						break;
					case 0x04:
						SendData_UserDef.Mode = 0;	//停止数据采集
						break;
					case 0x05:
						sleep_mode_enter();					//收到关机指令
						break;					
				}
			}
    }
		if(p_evt->type == BLE_NUS_EVT_COMM_STARTED)	
		{//如果使能发送特征通知
			NRF_LOG_INFO("Open uart CCCD");
			SendData_UserDef.BleDat_En = true;
		}
		if(p_evt->type == BLE_NUS_EVT_COMM_STOPPED)	
		{//如果关闭发送特征通知
			NRF_LOG_INFO("Close uart CCCD");
			SendData_UserDef.BleDat_En = false;
		}
}
/* USER CODE STOP PFP */






void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void timers_init(void)
{//定时器初始化
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
	
		err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, Nordic_period_timeout_handle);
    APP_ERROR_CHECK(err_code);		
	
		err_code = app_timer_create(&m_bat_timer_id, APP_TIMER_MODE_REPEATED, Nordic_battery_timeout_handle);
    APP_ERROR_CHECK(err_code);	
}

static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void gap_params_init(void)
{//GAP初始化
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
		
		//设置名称
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
		
		//连接参数设置：连接时间间隔
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


static void services_init(void)
{//蓝牙服务初始化
    uint32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};
		
    qwr_init.error_handler = nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    //声明串口传输服务
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_dat, &nus_init);
    APP_ERROR_CHECK(err_code);
		
		//声明电池电量服务
		bas_init();
		
		//声明设备信息服务
		EconHand_dis_init();
		
		//声明DFU服务
		dfu_init();
}


static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{//连接参数事件回调函数
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}



static void conn_params_error_handler(uint32_t nrf_error)
{//连接参数错误回调函数
    APP_ERROR_HANDLER(nrf_error);
}



static void conn_params_init(void)
{//连接参数初始化
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}



void sleep_mode_enter(void)
{
	ret_code_t	err_code;
	
  err_code = bsp_indication_set(BSP_INDICATE_IDLE);
  APP_ERROR_CHECK(err_code);
	
	/*唤醒GPIO设置*/
	nrf_gpio_cfg_sense_set(EXTI_KEY,NRF_GPIO_PIN_SENSE_LOW);				//按键低电平触发->唤醒
	nrf_gpio_cfg_sense_set(EXTI_Vcheck,NRF_GPIO_PIN_SENSE_HIGH);		//充电器接入高电平触发->唤醒
	
	app_timer_stop_all();
	ADS1299_Sleep_mode();				//睡眠1299芯片
	LSM6DSL_Sleep();						//睡眠LSM6DSL
	
	SPI_uninit();
	TWI_uninit();
	//nrfx_saadc_uninit();

	nrf_gpio_cfg_default(2);
	nrf_gpio_cfg_default(3);
	nrf_gpio_cfg_default(4);
	nrf_gpio_cfg_default(5);
	nrf_gpio_cfg_default(6);
	nrf_gpio_cfg_default(7);
	nrf_gpio_cfg_default(8);
	nrf_gpio_cfg_default(9);
	nrf_gpio_cfg_default(10);
	
	nrf_gpio_cfg_default(17);
	nrf_gpio_cfg_default(18);
	//nrf_gpio_cfg_default(19);		//充电器接入，不是失能
	nrf_gpio_cfg_default(20);				
	//nrf_gpio_cfg_default(21);		//按键输入，不能失能
		
	nrf_gpio_cfg_default(25);			//LED1
	nrf_gpio_cfg_default(26);			//LED2
		
	nrf_gpio_cfg_default(28);	
	nrf_gpio_cfg_default(29);
	nrf_gpio_cfg_default(30);	

	led_red_on;									
	nrf_delay_ms(900);
	led_red_off;									//红灯闪烁一下

	nrf_gpio_cfg_default(27);			//关机的时候需要LED工作，放到最后失能
	
	err_code = sd_power_system_off();
  APP_ERROR_CHECK(err_code);
}

void Sleep_withOut_red()
{
	ret_code_t	err_code;
	
  err_code = bsp_indication_set(BSP_INDICATE_IDLE);
  APP_ERROR_CHECK(err_code);
	
	/*唤醒GPIO设置*/
	nrf_gpio_cfg_sense_set(EXTI_KEY,NRF_GPIO_PIN_SENSE_LOW);				//按键低电平触发->唤醒
	nrf_gpio_cfg_sense_set(EXTI_Vcheck,NRF_GPIO_PIN_SENSE_HIGH);		//充电器接入高电平触发->唤醒
	
	app_timer_stop_all();
	ADS1299_Sleep_mode();				//睡眠1299芯片
	LSM6DSL_Sleep();						//睡眠LSM6DSL
	
	SPI_uninit();
	TWI_uninit();
	//nrfx_saadc_uninit();

	nrf_gpio_cfg_default(2);
	nrf_gpio_cfg_default(3);
	nrf_gpio_cfg_default(4);
	nrf_gpio_cfg_default(5);
	nrf_gpio_cfg_default(6);
	nrf_gpio_cfg_default(7);
	nrf_gpio_cfg_default(8);
	nrf_gpio_cfg_default(9);
	nrf_gpio_cfg_default(10);
	
	nrf_gpio_cfg_default(17);
	nrf_gpio_cfg_default(18);
	//nrf_gpio_cfg_default(19);		//充电器接入，不是失能
	nrf_gpio_cfg_default(20);				
	//nrf_gpio_cfg_default(21);		//按键输入，不能失能
		
	nrf_gpio_cfg_default(25);			//LED1
	nrf_gpio_cfg_default(26);			//LED2
		
	nrf_gpio_cfg_default(28);	
	nrf_gpio_cfg_default(29);
	nrf_gpio_cfg_default(30);	

	err_code = sd_power_system_off();
  APP_ERROR_CHECK(err_code);
}


static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{/*广播事件事件回调函数*/
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
//            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
//            APP_ERROR_CHECK(err_code);
				
						adv_started = true;
				
				
            break;
        case BLE_ADV_EVT_IDLE:
					
						adv_started = false;
            sleep_mode_enter();
				
            break;
        default:
            break;
    }
}



static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{/*协议栈事件回调函数*/
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:							//蓝牙连接事件
						NRF_LOG_INFO("Connected");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
				
				
						/*use code start*/
						LED_Reset_close();
						RGB_UserDef.nrf_RGB_State = 2;					
						SendData_UserDef.ble_Connect = true;

				break;

        case BLE_GAP_EVT_DISCONNECTED:					//蓝牙断开事件
            NRF_LOG_INFO("Disconnected");
            // LED indication will be changed when advertising starts.
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
				
				
						/*use code start*/
						LED_Reset_close();
						if(Vcheck_state == true)
						{//充电导致的蓝牙断开，直接进入充电指示灯状态
							RGB_UserDef.nrf_RGB_State = 3;
						}
						else
						{
							RGB_UserDef.nrf_RGB_State = 1;
						}
						
						SendData_UserDef.ble_Connect = false;

            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:		//1M,2M
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}



static void ble_stack_init(void)
{//协议栈初始化
    ret_code_t err_code;

		//协议栈回复使能应答
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

		//配置协议栈.2
		uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    //使能协议栈，导入配置参数
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    //注册协议栈回调函数
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}



void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{//GATT事件回调函数
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {//如果是MTU交换事件
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - 3;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}


void gatt_init(void)
{//GATT初始化--设置改变MTU参数
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);//从机MTU的值
    APP_ERROR_CHECK(err_code);
}



void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:		//蓝牙断开事件
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}


void uart_event_handle(app_uart_evt_t * p_event)
{/* 串口外设程序，臂环中用1299中断发送数据，不使用该内容	*/
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY://串口接收中断，PC发数据给从机
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') ||
                (data_array[index - 1] == '\r') ||
                (index >= m_ble_nus_max_data_len))//244字节
            {
                if (index > 1)
                {
                    NRF_LOG_DEBUG("Ready to send data over BLE NUS");
                    NRF_LOG_HEXDUMP_DEBUG(data_array, index);

                    do
                    {
                        uint16_t length = (uint16_t)index;
                        err_code = ble_nus_data_send(&m_dat, data_array, &length, m_conn_handle);//从机数据上传函数
                        if ((err_code != NRF_ERROR_INVALID_STATE) &&
                            (err_code != NRF_ERROR_RESOURCES) &&
                            (err_code != NRF_ERROR_NOT_FOUND))
                        {
                            APP_ERROR_CHECK(err_code);
                        }
                    } while (err_code == NRF_ERROR_RESOURCES);
                }

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}




static void advertising_init(void)
{//广播初始化
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

		//***************************广播包内容********************************//
    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
		
		//***************************广播扫描响应内容**************************//
    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

		//广播连接间隔和速度
    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}





//main function
int main(void)
{
    bool erase_bonds;
		ret_code_t err_code;
		
		/*使能中断之前，初始化异步SVCI接口到Bootloader*/
		err_code = ble_dfu_buttonless_async_svci_init();
	
		EconHand_BSP_LED_gpio_init();																			//LED灯GPIO初始化
		EconHand_BSP_power_gpio_init();																		//电源部分GPIO初始化

		/* ECG和IMU芯片外设初始化*/
		Nordic_TWI_init();																								//TWI	外设初始化
		LSM6DSL_init_ByUser();																						//LSM6DSL 芯片初始化																						
		ADS1292_gpio_config();																						//ADS1299 GPIO初始化
		SPI_User_init();																									//SPI 外设初始化
		initialize_ads();																									//ADS1299 芯片初始化
		
		/* 初始化完成后立刻睡眠，保证在广播状态下功耗尽可能的低	*/
		ADS_Enter_StandBY();																							//睡眠ADS1299
		LSM6DSL_Sleep();																									//睡眠姿态传感器
	
	
		/*	GPIOTE初始化	*/
		EconHand_GPIOTE_init();
	
    log_init();																												//log打印初始化
    timers_init();																										//软件定时器初始化
    power_management_init();																					//能量管理
    ble_stack_init();																									//协议栈初始化
    gap_params_init();																								//GAP初始化
    gatt_init();																											//GATT初始化
    services_init();																									//服务初始化
    advertising_init();																								//广播初始化
    conn_params_init();																								//连接参数更新初始化

		LED_Reset_close();																								//复位R G B三色灯
		period_timers_start();																						//开启周期定时器
		battery_timers_start();																						//开启电池电量定时器

		err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);		/*!! 会大幅度优化电源功耗 !!*/
		APP_ERROR_CHECK(err_code);
		
    advertising_start();																							//开始广播	
		NRF_LOG_INFO("Template example started.");


    for (;;)
    {
			idle_state_handle();
		}
}


/*
	用户主动下发断开蓝牙连接指令
	同时断开广播	
*/

//主程序中检测是否有用户主动下发断开协议栈指令
void	Observe_Close_BleSoftDevice()
{
	ret_code_t					err_code;
	if(isNeedDisconnect == true)
	{	//如果用户主动下发关闭指令
		isNeedDisconnect = false;
		//断开当前连接
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);                                     
		APP_ERROR_CHECK(err_code);	
		//同时宣告断开连接是用户自己主动操作，在advertising.h中识别到后不会在重启广播
		set_user_disconnect_flag();
	}
}


//主动断开广播和连接
void	APP_Close_BleADV()
{
	ret_code_t						err_code;
	//如果连接已经建立了
	if(((m_conn_handle != BLE_CONN_HANDLE_INVALID)) && (isNeedDisconnect == false))
	{		//m_conn_handle不等于无效句柄
		isNeedDisconnect = true;	//那就下发用户断开指令，同时不主动重连广播
	}

	//如果没有建立连接，同时广播页已经启动了
	if((adv_started == true) && (m_conn_handle == BLE_CONN_HANDLE_INVALID))
	{
		 NRF_LOG_INFO("STOP ADV"); 
		 err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);	//断开广播
		 if(err_code == NRF_SUCCESS)
		 {
			adv_started = false;
		 }
	}		
}





















































