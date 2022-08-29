/*********************************************************************
��˾���ƣ��Ϻ���ͨ����
�༭��	������
MPU6050�ο�ִ���ļ�

*********************************************************************/
//MPU6050�������򵥹��ܽ���
/*********************************************************************
1.��������ʽ���6���9�����ת������Ԫ����ŷ���Ǹ�ʽ����DMP��
2.���� 131 LSBs/��/sec ���ж���ȫ��вⷶΧΪ��250����500����1000 ���2000��/sec �� 3 ����ٶȸв���(������)
3.���ɿɳ�����ƣ���ΧΪ��2g����4g����8g �͡�16g �� 3 ����ٶȴ�����
4.�Դ������˶�����(DMP: Digital Motion Processing)����ɼ��� MCU ���ӵ��ں��������ݡ��в���ͬ���������Ƹ�Ӧ�ȵĸ���
5.�Դ�һ�������¶ȴ�����
6.�����ǹ���������5mA�������Ǵ���������5uA������������������500uA��������ʡ��ģʽ������40uA@10Hz
7.�Դ� 1024 �ֽ� FIFO�������ڽ���ϵͳ����
*********************************************************************/
//DMPʹ�ü��
/********************************************************************
ʵ��ʹ�ù����в�̫��ʹ�õ��������ͽ��ٶȵ�ԭʼ���ݣ�������ҪDMP������
�����(yaw)�������(roll)��������(pitch)
ͨ��MPU6050�ڲ��Դ���DMP���԰��˶�����ת��Ϊ��Ԫ���� Ȼ��ͨ����Ԫ���ó�ŷ����
*********************************************************************/
//��ֲ�ٷ���
/*********************************************************************
ͨ����ֲ�ٷ�����Ҫʵ��4������
i2c_write		i2c_read		delay_ms		get_ms



**********************************************************************/
#include "mpu6050.h"

extern short gyro[3], accel[3], sensors;
extern float Pitch,Roll,Yaw;
extern float q0,q1,q2,q3;
extern uint16_t OutPut_Buffer[42];
extern uint8_t IMUFlag;	


uint8_t IMU_Data_buf[6]={0};





//�����Ƿ�������
static	signed	char	gyro_orientation[9] = {1, 0, 0,
																						 0,	1, 0,
																						 0, 0, 1};

																						 
#define DEFAULT_MPU_HZ  (100)		//MPU����Ƶ��100Hz
																						 
#ifdef	Func_Basic			 //�Ա���DMP���к���
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
void get_ms(unsigned long *count)	//�պ���
{

}
#endif
#ifdef	User_funZDYZ		 //����ԭ���µ�3���ο���������
/******************************************************************************
�������ܣ�MPU6050�Բ⺯��
	����ֵ��0������
          ������ʧ��
		���ߣ�Sok��
******************************************************************************/
uint8_t	run_self_test()
{
	int result;
	long gyro[3],	accel[3];
	//�����Բ⺯����MPUоƬ��Ҫ��ֱZ�ᣬƽ����������
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
�������ܣ�����ת��
		���ߣ�Sok��
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
�������ܣ����������Ƿ���
		���ߣ�Sok��
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


#ifdef 	User_DMPdata		 //DMP���ݶ�д����
/******************************************************************************
�������ܣ�MPU6050����DMP��ʼ��
		���ߣ�Sok��
******************************************************************************/
void DMP_init()
{
	uint8_t temp[1] = {0};
	i2cRead(0x68,0x75,1,temp);			//��ȡWHO AM I�Ĵ���	�ļĴ��������豸��ʶ��֤ �ϵ縴λbit6 - bir1 ��	110100  bit0=0�������ϵ縴λ��ɶ�ȡ��0x68
	if(temp[0]!=0x68)								//�����ʼ��ʧ��
	{NVIC_SystemReset();}						//ϵͳ�ڲ���λ
	if(!mpu_init())									//ϵͳ�Դ�mpu��ʼ�� 50HZ ��2000ps/n	��2G/n
	{
	  if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))				//��XYZ�ǶȺͼ��ٶȴ�����

	  	 //printf("mpu_set_sensor complete ......\r\n");
	  if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))			//��FIFO

	  	 //printf("mpu_configure_fifo complete ......\r\n");			
	  if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))									//���ò���Ƶ��100hz

	  	 //printf("mpu_set_sample_rate complete ......\r\n");
	  if(!dmp_load_motion_driver_firmware())										//����DMP�̼�

	  	//printf("dmp_load_motion_driver_firmware complete ......\r\n");
	  if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))

	  	 //printf("dmp_set_orientation complete ......\r\n");			//���������Ƿ���		YZX
	  if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
	        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
	        DMP_FEATURE_GYRO_CAL))															//���������ǹ���

	  	 //printf("dmp_enable_feature complete ......\r\n");
	  if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))										//����DMP������ʣ���󲻳���200hz
	  	 //printf("dmp_set_fifo_rate complete ......\r\n");

	  run_self_test();																					//�Լ�
	  if(!mpu_set_dmp_state(1))																	//ʹ��DMP
	  {}
	  	 //printf("mpu_set_dmp_state complete ......\r\n");
	}
}

/******************************************************************************
�������ܣ���ȡMPU6050����DMP����̬��Ϣ
		���ߣ�Sok��
******************************************************************************/
void	Read_DMP()
{
	  unsigned long sensor_timestamp;
		unsigned char more;
		long quat[4];
		//dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more)
		//��ȡ����������
		//��ȡ���ٶ�����
		//��ȡ��Ԫ������
		//ʱ��� ms
		//��������FIFO�ж�ȡ������
		//ʣ�౨��
		dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
	
		if (sensors & INV_WXYZ_QUAT )
		{
			q0=quat[0] / q30;
			q1=quat[1] / q30;
			q2=quat[2] / q30;
			q3=quat[3] / q30;
			//��Ԫ��תŷ����
			Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; 	
			Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
			Yaw  = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
		}
}

/******************************************************************************
�������ܣ���ȡMPU6050�����¶ȴ�����
	����ֵ���¶�����100��
		���ߣ�Sok��
******************************************************************************/
short MPU_Get_Temperature()
{
	uint8_t buf[2];		//���ݻ�������
	short	raw;
	float temp;
	i2cRead(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf);
	raw = ((uint16_t)buf[0]|buf[1]);
	temp = 36.53+((double)raw)/340;
	return temp*100;;
}
#endif


#ifdef 	User_Translate	 //MCU���ͺ���
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





//asin 	ȡֵ��Χ		[-pi/2 ,			 pi/2] 
//atan2	ȡֵ��Χ		[-pi	 ,			 pi  ]

//pitch	ȡֵ��Χ 		[-8996.1      8996.1]
//Roll 	ȡֵ��Χ		[-17992.2		 17992.2]
//Yaw	 	ȡֵ��Χ		[-17992.2		 17992.2]








































