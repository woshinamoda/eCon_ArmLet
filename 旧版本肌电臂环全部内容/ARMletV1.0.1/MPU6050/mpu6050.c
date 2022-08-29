/*********************************************************************
公司名称：上海念通智能
编辑人	：李旺
MPU6050参考执行文件

*********************************************************************/
//MPU6050传感器简单功能介绍
/*********************************************************************
1.以数字形式输出6轴或9轴的旋转矩阵，四元数，欧拉角格式（开DMP）
2.具有 131 LSBs/°/sec 敏感度与全格感测范围为±250、±500、±1000 与±2000°/sec 的 3 轴角速度感测器(陀螺仪)
3.集成可程序控制，范围为±2g、±4g、±8g 和±16g 的 3 轴加速度传感器
4.自带数字运动处理(DMP: Digital Motion Processing)引擎可减少 MCU 复杂的融合演算数据、感测器同步化、姿势感应等的负荷
5.自带一个数字温度传感器
6.陀螺仪工作电流：5mA，陀螺仪待机电流：5uA；加速器工作电流：500uA，加速器省电模式电流：40uA@10Hz
7.自带 1024 字节 FIFO，有助于降低系统功耗
*********************************************************************/
//DMP使用简介
/********************************************************************
实际使用过程中不太会使用到传感器和角速度的原始数据，而是需要DMP计算后的
航向角(yaw)、横滚角(roll)、俯仰角(pitch)
通过MPU6050内部自带的DMP可以把运动数据转换为四元数， 然后通过四元数得出欧拉角
*********************************************************************/
//移植官方库
/*********************************************************************
通过移植官方库主要实现4个函数
i2c_write		i2c_read		delay_ms		get_ms



**********************************************************************/
#include "mpu6050.h"

extern short gyro[3], accel[3], sensors;
extern float Pitch,Roll,Yaw;
extern float q0,q1,q2,q3;
extern uint16_t OutPut_Buffer[42];
extern uint8_t IMUFlag;	


uint8_t IMU_Data_buf[6]={0};





//陀螺仪方向设置
static	signed	char	gyro_orientation[9] = {1, 0, 0,
																						 0,	1, 0,
																						 0, 0, 1};

																						 
#define DEFAULT_MPU_HZ  (100)		//MPU工作频率100Hz
																						 
#ifdef	Func_Basic			 //自编译DMP库中函数
uint8_t i2cWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data)
{
	return HAL_I2C_Mem_Write(&hi2c2, (slave_addr<<1), (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 0xFF);
}

uint8_t i2cRead(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data)
{
  return HAL_I2C_Mem_Read(&hi2c2, (slave_addr<<1), (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 0xFF);
}
void delay_ms(uint32_t Delay)
{
	HAL_Delay(Delay);
}
void get_ms(unsigned long *count)	//空函数
{

}
#endif
#ifdef	User_funZDYZ		 //正点原子下的3个参考基础函数
/******************************************************************************
函数功能：MPU6050自测函数
	返回值：0：正常
          其他：失败
		作者：Sok旺
******************************************************************************/
uint8_t	run_self_test()
{
	int result;
	long gyro[3],	accel[3];
	//引用自测函数，MPU芯片需要垂直Z轴，平行于重力侧
	result = mpu_run_self_test(gyro, accel);
	if (result == 0x3) 
	{
		/* Test passed. We can trust the gyro data here, so let's push it down
		* to the DMP.
		*/
		float sens;
		unsigned short accel_sens;
		mpu_get_gyro_sens(&sens);
		gyro[0] = (long)(gyro[0] * sens);
		gyro[1] = (long)(gyro[1] * sens);
		gyro[2] = (long)(gyro[2] * sens);
		dmp_set_gyro_bias(gyro);
		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		dmp_set_accel_bias(accel);
		return 0;
	}else return 1;
}																					 
						
/******************************************************************************
函数功能：方向转换
		作者：Sok旺
******************************************************************************/		
unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}	


/******************************************************************************
函数功能：控制陀螺仪方向
		作者：Sok旺
******************************************************************************/																				 
unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar; 
    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}																						 																				 
#endif


#ifdef 	User_DMPdata		 //DMP数据读写函数
/******************************************************************************
函数功能：MPU6050内置DMP初始化
		作者：Sok旺
******************************************************************************/
void DMP_init()
{
	uint8_t temp[1] = {0};
	i2cRead(0x68,0x75,1,temp);			//读取WHO AM I寄存器	改寄存器用于设备标识验证 上电复位bit6 - bir1 ：	110100  bit0=0代表都，上电复位完成读取到0x68
	if(temp[0]!=0x68)								//如果初始化失败
	{NVIC_SystemReset();}						//系统内部复位
	if(!mpu_init())									//系统自带mpu初始化 50HZ ±2000ps/n	±2G/n
	{
	  if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))				//打开XYZ角度和加速度传感器

	  	 //printf("mpu_set_sensor complete ......\r\n");
	  if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))			//打开FIFO

	  	 //printf("mpu_configure_fifo complete ......\r\n");			
	  if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))									//设置采样频率100hz

	  	 //printf("mpu_set_sample_rate complete ......\r\n");
	  if(!dmp_load_motion_driver_firmware())										//加载DMP固件

	  	//printf("dmp_load_motion_driver_firmware complete ......\r\n");
	  if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))

	  	 //printf("dmp_set_orientation complete ......\r\n");			//设置陀螺仪方向		YZX
	  if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
	        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
	        DMP_FEATURE_GYRO_CAL))															//设置陀螺仪功能

	  	 //printf("dmp_enable_feature complete ......\r\n");
	  if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))										//设置DMP输出速率，最大不超过200hz
	  	 //printf("dmp_set_fifo_rate complete ......\r\n");

	  run_self_test();																					//自检
	  if(!mpu_set_dmp_state(1))																	//使能DMP
	  {}
	  	 //printf("mpu_set_dmp_state complete ......\r\n");
	}
}

/******************************************************************************
函数功能：读取MPU6050内置DMP的姿态信息
		作者：Sok旺
******************************************************************************/
void	Read_DMP()
{
	  unsigned long sensor_timestamp;
		unsigned char more;
		long quat[4];
		//dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more)
		//读取陀螺仪数据
		//读取加速度数据
		//读取四元数数据
		//时间戳 ms
		//传感器从FIFO中读取的掩码
		//剩余报文
		dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
	
		if (sensors & INV_WXYZ_QUAT )
		{
			q0=quat[0] / q30;
			q1=quat[1] / q30;
			q2=quat[2] / q30;
			q3=quat[3] / q30;
			//四元数转欧拉角
			Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; 	
			Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
			Yaw  = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
		}
}

/******************************************************************************
函数功能：读取MPU6050内置温度传感器
	返回值：温度扩大100倍
		作者：Sok旺
******************************************************************************/
short MPU_Get_Temperature()
{
	uint8_t buf[2];		//数据缓冲数组
	short	raw;
	float temp;
	i2cRead(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf);
	raw = ((uint16_t)buf[0]|buf[1]);
	temp = 36.53+((double)raw)/340;
	return temp*100;;
}
#endif


#ifdef 	User_Translate	 //MCU发送函数
void IMU_Data_Send()
{
	IMU_Data_buf[0] =	(short)(Pitch*100) & 0xff;
	IMU_Data_buf[1] =	(short)(Pitch*100) >>8 & 0xff;
	IMU_Data_buf[2] =	(short)(Roll*100)  & 0xff;
	IMU_Data_buf[3] =	(short)(Roll*100) >>8 & 0xff;
	IMU_Data_buf[4] =	(short)(Yaw*100)  & 0xff;
	IMU_Data_buf[5] =	(short)(Yaw*100) >>8 & 0xff;	
	IMUFlag = 1;
}
#endif





//asin 	取值范围		[-pi/2 ,			 pi/2] 
//atan2	取值范围		[-pi	 ,			 pi  ]

//pitch	取值范围 		[-8996.1      8996.1]
//Roll 	取值范围		[-17992.2		 17992.2]
//Yaw	 	取值范围		[-17992.2		 17992.2]








































