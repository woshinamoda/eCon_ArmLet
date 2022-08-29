
#include "stm32f4xx.h"
#include <stdio.h>
#include "inv_mpu.h"
#include "stm_I2C.h"
#include "mpu9150.h"
#include "stm_delay.h"
#include "bsp_usart.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#define DEFAULT_MPU_HZ  50
#define MPU9150_Addr            0x68
#define P_M_1                   0x6B
#define Accel_FILTER_260HZ		260
#define Accel_FILTER_98HZ		0x98
#define Accel_FILTER_44HZ		0x44
#define Accel_FILTER_21HZ		15
#define Gyro_Xout_H		        0x43
#define Bypass_Enable_Cfg		0x37
#define User_Ctrl               0x6A
#define Compass_Addr            0x0C
#define Compass_CNTL            0x0A
#define Accel_Xout_H		    0x3B
#define Compass_ST1             0x02
#define	Compass_HXL             0x03
#define q30  1073741824.0f
//#define q30  1
signed short init_ax, init_ay, init_az;
signed short init_gx, init_gy, init_gz;
signed short init_mx, init_my, init_mz;

static float MXgain = 0;
static float MYgain = 0;
static float MZgain = 0;
static float MXoffset = 0;
static float MYoffset = 0;
static float MZoffset = 0;
static short maxMagX = 0;
static short minMagX = 0;
static short maxMagY = 0;
static short minMagY = 0;
static short maxMagZ = 0;
static short minMagZ = 0;

static u8 m=6,n=0;
u8 MPU_Sent[12]={0};
uint8_t tmp[7]={0};
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};
/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static  unsigned short inv_row_2_scale(const signed char *row)
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
static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
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

/* Handle sensor on/off combinations. */

static void run_self_test(void)
{
    int result;
//    char test_packet[4]2 = {0};
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
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
		//PrintChar("setting bias succesfully ......\n");
    }
	else
	{
		//PrintChar("bias has not been modified ......\n");
	}

    /* Report results. */
}

/*******************************************************************************
* Function Name  : get_mpu9150_data
* Description    : 读取mpu9150的加速度计 陀螺仪 磁力计数据并做校准和滤波.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int get_mpu9150_data(u8 * mpudata)
{
	unsigned long sensor_timestamp;
	unsigned char data_write[14];
	short gyro[3], accel[3],mag[3], sensors;
	unsigned char more;
	long quat[4];
	float Yaw,Roll,Pitch;
	float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
	unsigned char IIC_cnt[2];
	unsigned char IIC_cnt1[2];
	IIC_cnt[0]=0x20;
	IIC_cnt[1]=0x00;
	IIC_cnt1[0]=0x00;
	IIC_cnt1[0]=0x02;
	
	if(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more)) return 0;
	//while(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more)) {;}
	//zhaobin:0312
	//mpu_reset_fifo();
	if (sensors & INV_WXYZ_QUAT )
	{
		
		q0=quat[0] / q30;
		q1=quat[1] / q30;
		q2=quat[2] / q30;
		q3=quat[3] / q30;
		
		Yaw=atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
		Pitch=asin(2 * q1 * q3 - 2 * q0* q2)* 57.3; // pitch
		Roll=atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
	
		mpudata[0]=((short)(Pitch*100))>>8;
		mpudata[1]=((short)(Pitch*100))&0x00ff;
		mpudata[2]=((short)(Roll*100))>>8;
		mpudata[3]=((short)(Roll*100))&0x00ff;	
		mpudata[4]=((short)(Yaw*100))>>8;
		mpudata[5]=((short)(Yaw*100))&0x00ff;	
		//printf("p:%f  r:%f y: %f  t:%ld re:%u\n",Pitch,Roll,Yaw,sensor_timestamp,more);
	}
	if(sensors & INV_XYZ_GYRO){
		for (n=3;n<=5;n++)
	  {
	     mpudata[2*n]=gyro[n-3]>>8; 
	     mpudata[2*n+1]=gyro[n-3]&0x00ff;
	  }
	}
	if(sensors & INV_XYZ_ACCEL){
		for (n=6;n<=8;n++)
	  {
	     mpudata[2*n]=accel[n-6]>>8; 
	     mpudata[2*n+1]=accel[n-6]&0x00ff;
	  }
	}
	/*
	if(!i2cread(MPU9150_Addr, Accel_Xout_H, 14, data_write))
   {
	  accel[0]=(((signed short int)data_write[0])<<8) | data_write[1];
	  accel[1]=(((signed short int)data_write[2])<<8) | data_write[3];
    accel[2]=(((signed short int)data_write[4])<<8) | data_write[5];
	  gyro[0] =(((signed short int)data_write[8])<<8) | data_write[9];
    gyro[1] =(((signed short int)data_write[10])<<8) | data_write[11];
	  gyro[2] =(((signed short int)data_write[12])<<8) | data_write[13];
		for (n=3;n<=5;n++)
	  {
	     MPU_Sent[2*n]=gyro[n-3]>>8; 
	     MPU_Sent[2*n+1]=gyro[n-3]&0x00ff;
	  }
		for (n=6;n<=8;n++)
	  {
	     MPU_Sent[2*n]=accel[n-6]>>8; 
	     MPU_Sent[2*n+1]=accel[n-6]&0x00ff;
	  }*/
		 /*
		 for (n=3;n<=5;n++)
	  {
	     mpudata[2*n]=accel[n-3]>>8; 
	     mpudata[2*n+1]=accel[n-3]&0x00ff;
				//mpudata[2*n]=0;
			 //mpudata[2*n+1]=0;
	  }
		for (n=6;n<=8;n++)
	  {
	     MPU_Sent[2*n]=gyro[n-6]>>8; 
	     MPU_Sent[2*n+1]=gyro[n-6]&0x00ff;
			 //mpudata[2*n]=0;
			 //mpudata[2*n+1]=0;
	  }*/
	 //}
	init_ax=accel[0];
	init_gx=gyro[0];
	return 1;
}


/*******************************************************************************
读取compass数据，在初始化mpu9150后先读几次mag的数据，因为前几次读取的mag数据有
错误，芯片bug	
*******************************************************************************/
int Init_MPU9150_Mag(void)
{
  unsigned char data_write[3];
  
  data_write[0]=0x02;       
  data_write[1]=0x00;
  data_write[2]=0x01;
  
  i2cwrite(MPU9150_Addr, Bypass_Enable_Cfg, 1, data_write);	 //开启bypass
  //delay_ms(10);                     
  i2cwrite(MPU9150_Addr, User_Ctrl, 1, data_write+1);	 //关闭MPU9150的I2C_MASTER模式，必须要有这句
  //delay_ms(10);
  i2cwrite(Compass_Addr, Compass_CNTL, 1, data_write+2);	 //开启compass：single measurement mode

  return 0;  
}


void init_mpu9150(void)
{
  int result;
  unsigned char data_write[1];

  result = mpu_init();
	if(!result)
  {
		
	 //mpu_set_sensor
	  mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
		
	  //mpu_configure_fifo
	  mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);

	  //mpu_set_sample_rate
	  mpu_set_sample_rate(DEFAULT_MPU_HZ);
		
		//mpu_set_gyro_fsr(500);

	  //设置加速度计测量范围：+-4G
	  //mpu_set_accel_fsr(4);
		
	  //dmp_load_motion_driver_firmvare
	  dmp_load_motion_driver_firmware();

	  //dmp_set_orientation
	  dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));

	  //dmp_enable_feature
	  dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_RAW_GYRO |DMP_FEATURE_GYRO_CAL);

	  //dmp_set_fifo_rate
	  dmp_set_fifo_rate(DEFAULT_MPU_HZ);

	  run_self_test();
	  mpu_set_dmp_state(1);
//Init_MPU9150_Mag();
		/*
	  //开启加速度计、陀螺仪
	  mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
		mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL| INV_XYZ_COMPASS);
	  //设置MPU9150的时钟源为GX的PLL
	  data_write[0]=0x01;				//GX_PLL:0x01
	  i2cwrite(MPU9150_Addr, P_M_1, 1, data_write);

	  //设置陀螺仪测量范：+-500度/s
	  mpu_set_gyro_fsr(500);

	  //设置加速度计测量范围：+-4G
	  mpu_set_accel_fsr(4);

	  //设置加速度计的低通滤波器，防震动
	  mpu_set_lpf(Accel_FILTER_21HZ);

	  //设置采样率100Hz
	  mpu_set_sample_rate(DEFAULT_MPU_HZ);
		
	  Init_MPU9150_Mag();
	  //printf("mpu initialization complete ......\n ");
		
		//zhaobin 0305:加入DMP的初始化程序

		//dmp_load_motion_driver_firmvare
	  dmp_load_motion_driver_firmware();

	  //dmp_set_orientation
		dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));

	  //dmp_enable_feature
		
	  //dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
	        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
	        DMP_FEATURE_GYRO_CAL);
		dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT| DMP_FEATURE_SEND_RAW_ACCEL  |
	        DMP_FEATURE_SEND_CAL_GYRO |DMP_FEATURE_GYRO_CAL);
	  //dmp_set_fifo_rate
	  dmp_set_fifo_rate(200);
	  run_self_test();
	  mpu_set_dmp_state(1);*/

  }
  else
  {
      printf("mpu initialization error......\n");
	  while(1);
  }
  //printf("Start Calculating .....\n");
}


/*******************************************************************************
读取compass数据，用于compass校准	
*******************************************************************************/
void Read_MPU9150_Mag(void)
{
  signed short int mag[3];
  unsigned char tmp[7], data_write[1];
  
  tmp[6]=0x00;
  data_write[0]=0x01;
  i2cread(Compass_Addr, Compass_ST1, 1, tmp+6);
  if(tmp[6] == 1)
  {
    i2cread(Compass_Addr, Compass_HXL, 6, tmp);
	  mag[0] = (((signed short int)tmp[1]) << 8) | tmp[0];
    mag[1] = (((signed short int)tmp[3]) << 8) | tmp[2];
    mag[2] = (((signed short int)tmp[5]) << 8) | tmp[4];

	  mag[0] = ((long)mag[0] * mag_sens_adj_val[0]) >> 7;  //灵敏度调整
    mag[1] = ((long)mag[1] * mag_sens_adj_val[1]) >> 7;
    mag[2] = ((long)mag[2] * mag_sens_adj_val[2]) >> 7;

	  init_mx = mag[1];		//转换坐标轴				
    init_my = mag[0];
    init_mz =-mag[2];
	  i2cwrite(Compass_Addr, Compass_CNTL, 1, data_write);	 //开启compass：single measurement mode
  }  
}

/*******************************************************************************
得到mag的Xmax、Xmin、Ymax、Ymin、Zmax、Zmin	
*******************************************************************************/
void get_compass_bias(void)
{
  Read_MPU9150_Mag();

  if(init_mx > maxMagX)
  maxMagX = init_mx;
  if(init_mx < minMagX)
  minMagX = init_mx;

  if(init_my > maxMagY)
  maxMagY = init_my;
  if(init_my < minMagY)
  minMagY = init_my;

  if(init_mz > maxMagZ)
  maxMagZ = init_mz;
  if(init_mz < minMagZ)
  minMagZ = init_mz;
  //printf("maxMagX=%d, minMagX=%d, maxMagY=%d, minMagY=%d, maxMagZ=%d, minMagZ=%d \n", 
          //maxMagX, minMagX, maxMagY, minMagY, maxMagZ, minMagZ);  
}
/*******************************************************************************
空间校准compass	
*******************************************************************************/
void compass_calibration(void)
{ //将有最大响应的轴的增益设为1
  if(((maxMagX - minMagX) >= (maxMagY - minMagY)) && ((maxMagX - minMagX) >= (maxMagZ - minMagZ)))
  {
    MXgain = 1.0;
	MYgain = (maxMagX - minMagX) / (maxMagY - minMagY);
	MZgain = (maxMagX - minMagX) / (maxMagZ - minMagZ);
	MXoffset = -0.5 * (maxMagX + minMagX);
	MYoffset = -0.5 * MYgain * (maxMagY + minMagY);
	MZoffset = -0.5 * MZgain * (maxMagZ + minMagZ);	 
  }
  if(((maxMagY - minMagY) > (maxMagX - minMagX)) && ((maxMagY - minMagY) >= (maxMagZ - minMagZ)))
  {
    MXgain = (maxMagY - minMagY) / (maxMagX - minMagX);
	MYgain = 1.0;
	MZgain = (maxMagY - minMagY) / (maxMagZ - minMagZ);
	MXoffset = -0.5 * MXgain * (maxMagX + minMagX);
	MYoffset = -0.5 * (maxMagY + minMagY);
	MZoffset = -0.5 * MZgain * (maxMagZ + minMagZ);    
  }
  if(((maxMagZ - minMagZ) > (maxMagX - minMagX)) && ((maxMagZ - minMagZ) > (maxMagY - minMagY)))
  {
    MXgain = (maxMagZ - minMagZ) / (maxMagX - minMagX);
	MYgain = (maxMagZ - minMagZ) / (maxMagY - minMagY);
	MZgain = 1.0;
	MXoffset = -0.5 * MXgain * (maxMagX + minMagX);
	MYoffset = -0.5 * MYgain * (maxMagY + minMagY);
	MZoffset = -0.5 * (maxMagZ + minMagZ);    
  }
  //printf("MXgain=%f, MYgain=%f, MZgain=%f, MXoffset=%f, MYoffset=%f, MZoffset=%f \n", 
          //MXgain, MYgain, MZgain, MXoffset, MYoffset, MZoffset);         
}


