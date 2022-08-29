#include "nrf_lsm6dsl.h"
#include "lsm6dsltr.h"

//caculuar by IMU
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

lsm6dsl_ctx_t	dev_ctx;

//TWI��������ʵ��ID��ID�������Ŷ�Ӧ��0:TWI0		1:TWI1
#define		TWI_INSTANCE_ID		0

//����TWI������ɱ�־
static volatile bool	m_xfer_done = false;
//����TWI����ʵ��������m_twi
static	const	nrf_drv_twi_t	m_twi=	NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);



void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{//TWI�¼�������
	m_xfer_done = true;		//����SPI�������
}

void Nordic_TWI_init(void)
{//TWI��ʼ��
    ret_code_t err_code;
	    //���岢��ʼ��TWI���ýṹ��
    const nrf_drv_twi_config_t twi_config = {
		    .scl               = TWI_SCL_M,  //����TWI SCL����
       .sda                = TWI_SDA_M,  //����TWI SDA����
       .frequency          = NRF_DRV_TWI_FREQ_400K, //TWI����
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //TWI���ȼ�
       .clear_bus_init     = false//��ʼ���ڼ䲻����9��SCLʱ��
    };
    //��ʼ��TWI
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
		//��鷵�صĴ������
    APP_ERROR_CHECK(err_code);
    //ʹ��TWI
    nrf_drv_twi_enable(&m_twi);
}


void TWI_uninit()
{
	nrf_drv_twi_uninit(&m_twi);
}




/* ����ST�ٷ�����-------------------------------------------------------------------------------------------------------*/
/* private ST official Program code for IMU begin */
int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp,  uint16_t len)
{
	  ret_code_t err_code;
	  uint8_t tx_buf[len + 1];
	  //׼��д�������
		tx_buf[0] = Reg;
		for(uint8_t i=1; i<len+1; i++)
		{
			tx_buf[i] = Bufp[i-1];
		}	
	  //TWI������ɱ�־����Ϊfalse
		m_xfer_done = false;
		//д������
    err_code = nrf_drv_twi_tx(&m_twi, LSM6D_ADDRESS_H, tx_buf, len+1, false);
	  //�ȴ�TWI���ߴ������
		while (m_xfer_done == false){}
		return 0;
}


int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	  ret_code_t err_code;
		uint8_t	tx_buf[1];
		tx_buf[0] = Reg;
	  //TWI������ɱ�־����Ϊfalse
		m_xfer_done = false;
	  err_code = nrf_drv_twi_tx(&m_twi, LSM6D_ADDRESS_H, tx_buf, 1, true);
	  //�ȴ�TWI���ߴ������
		while (m_xfer_done == false){}
		//TWI������ɱ�־����Ϊfalse
		m_xfer_done = false;
	  err_code = nrf_drv_twi_rx(&m_twi, LSM6D_ADDRESS_H, Bufp, len);
		//�ȴ�TWI���ߴ������
		while (m_xfer_done == false){}
		return 0;
}


void LSM6DSL_init()
{
	lsm6dsl_int1_route_t int_1_reg;
	
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;

	/*check device ID*/
  lsm6dsl_device_id_get(&dev_ctx, &IMU_UserDef.whoamI);
  if ( IMU_UserDef.whoamI != LSM6DSL_ID )
    while(1); 
	
	/*Restore default configuration*/
  lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);
  do {
    lsm6dsl_reset_get(&dev_ctx, &IMU_UserDef.rst);
  } while (IMU_UserDef.rst);	
	
 /* Enable Block Data Update */
  lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);	
	
  /* Set Output Data Rate */
  lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_52Hz);	//���ٶ�����
  lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_52Hz);	//���ٶ�����
	
  /* Set full scale */  
																														//���渡��ת��ע�ⵥλ
  lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_2g);					//���ٶ�����		
  lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_2000dps);			//���ٶ�����
	
  /* Configure filtering chain(No aux interface)-------------------------- */  
  /* Accelerometer - analog filter */
  lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);

  /* Accelerometer - LPF1 + LPF2 path */   
  lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

  /* Gyroscope - filtering chain */
  lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);

	lsm6dsl_data_ready_mode_set(&dev_ctx, LSM6DSL_DRDY_PULSED);	//����DRDY

	/* Enable interrupt generation on DRDY INT1 pin */
  lsm6dsl_pin_int1_route_get(&dev_ctx, &int_1_reg);
  int_1_reg.int1_drdy_g = PROPERTY_ENABLE;
  int_1_reg.int1_drdy_xl = PROPERTY_ENABLE;
  lsm6dsl_pin_int1_route_set(&dev_ctx, int_1_reg);
}
/* private ST official Program code for IMU end  */

/* �����ֲ��Զ������-------------------------------------------------------------------------------------------------- */
/* private User code begin */
uint8_t LSM6DSL_Write_Byte(uint8_t LSM6D_reg, uint8_t value)
{//��Ӧ�Ĵ���дһ��byte����
	  ret_code_t err_code;
	  uint8_t tx_buf[2];	
	  tx_buf[0] = LSM6D_reg;
    tx_buf[1] = value;
    err_code = nrf_drv_twi_tx(&m_twi, LSM6D_ADDRESS_H, tx_buf, 2, false);
		APP_ERROR_CHECK(err_code);
}


uint8_t LSM6DSL_ReadOneByte(uint8_t LSM6D_reg)
{//��Ӧ�Ĵ�����һ��byte����
	  ret_code_t err_code;
	
		uint8_t	get_buf[1];
		uint8_t	tx_buf[1];	
		tx_buf[0] = LSM6D_reg;
	  nrf_drv_twi_tx(&m_twi, LSM6D_ADDRESS_H, tx_buf, 1, true);
	  nrf_drv_twi_rx(&m_twi, LSM6D_ADDRESS_H, get_buf, 1);
		return get_buf[0];
}


void LSM6DSL_init_ByUser()
{//LSM6DSL��ʼ��
	LSM6DSL_Write_Byte(LSM6DSL_CTRL1_XL, 0x30);						//52Hz	2g
	LSM6DSL_Write_Byte(LSM6DSL_CTRL2_G, 0x3C);						//52Hz	��2000
	LSM6DSL_Write_Byte(LSM6DSL_INT1_CTRL, 0x03);					//����INT1
	LSM6DSL_Write_Byte(LSM6DSL_DRDY_PULSE_CFG_G, 0x80);		//INT1��Ϊ����
	LSM6DSL_Write_Byte(LSM6DSL_CTRL6_C, 0x10);						//XL_HM_MODE��1.���ٶȿ����͹���ģʽP65
	LSM6DSL_Write_Byte(LSM6DSL_CTRL7_G, 0xE0);						//G_HM_MODEֵ1�����ٶȿ����͹���ģʽP66
}


void Lsm_Get_Rawacc(void)  
{//��ȡ���ٶ�ֵ
	uint8_t res_go;
	uint8_t buf[6];
	
	res_go = LSM6DSL_ReadOneByte(LSM6DSL_STATUS_REG);
	if((res_go & 0x01)!=0)
	{ //XLDAλΪ1�����µļ��ٶ�ֵ
		buf[0] = LSM6DSL_ReadOneByte(LSM6DSL_OUTX_H_XL);
		buf[1] = LSM6DSL_ReadOneByte(LSM6DSL_OUTX_L_XL);
		buf[2] = LSM6DSL_ReadOneByte(LSM6DSL_OUTY_H_XL);
		buf[3] = LSM6DSL_ReadOneByte(LSM6DSL_OUTY_L_XL);
		buf[4] = LSM6DSL_ReadOneByte(LSM6DSL_OUTZ_H_XL);
		buf[5] = LSM6DSL_ReadOneByte(LSM6DSL_OUTZ_L_XL);		
		IMU_UserDef.ax=(buf[0]<<8)|buf[1];
		IMU_UserDef.ay=(buf[2]<<8)|buf[3];
		IMU_UserDef.az=(buf[4]<<8)|buf[5];	
	}
}


void Lsm_Get_Rawgryo(void)
{//��ȡ���ٶ�ֵ
	uint8_t res_go;
	uint8_t	buf[6];
	
	res_go = LSM6DSL_ReadOneByte(LSM6DSL_STATUS_REG);
	if((res_go & 0x02)!=0)
	{ //GDAλΪ1�����µĽ��ٶ�ֵ
		buf[0] = LSM6DSL_ReadOneByte(LSM6DSL_OUTX_H_G);
		buf[1] = LSM6DSL_ReadOneByte(LSM6DSL_OUTX_L_G);
		buf[2] = LSM6DSL_ReadOneByte(LSM6DSL_OUTY_H_G);
		buf[3] = LSM6DSL_ReadOneByte(LSM6DSL_OUTY_L_G);
		buf[4] = LSM6DSL_ReadOneByte(LSM6DSL_OUTZ_H_G);
		buf[5] = LSM6DSL_ReadOneByte(LSM6DSL_OUTZ_L_G);		
		IMU_UserDef.gx=(buf[0]<<8)|buf[1];
		IMU_UserDef.gy=(buf[2]<<8)|buf[3];
		IMU_UserDef.gz=(buf[4]<<8)|buf[5];	
	}	
}


void Get_LSM6D_Data()
{
	Lsm_Get_Rawacc();
	Lsm_Get_Rawgryo();
	//���ٶȷ�Χ��2g 1LSb = 0.061mg at ��2g
	//ת�������ݵ�λ mg/LSB
	IMU_UserDef.acc_x = lsm6dsl_from_fs2g_to_mg(IMU_UserDef.ax);
	IMU_UserDef.acc_y = lsm6dsl_from_fs2g_to_mg(IMU_UserDef.ay);
	IMU_UserDef.acc_z = lsm6dsl_from_fs2g_to_mg(IMU_UserDef.az);
	/* ���ٶȵõ��ĵ�λ mg*/
	//���ٶȷ�Χ1LSB = 70mpds at �� 2000dps
	//ת����λ mdps/LSB
	IMU_UserDef.gyr_x = lsm6dsl_from_fs2000dps_to_mdps(IMU_UserDef.gx);
	IMU_UserDef.gyr_y = lsm6dsl_from_fs2000dps_to_mdps(IMU_UserDef.gy);
	IMU_UserDef.gyr_z = lsm6dsl_from_fs2000dps_to_mdps(IMU_UserDef.gz);
	/* ���ٶȵõ��ĵ�λ dps*/
}



void	LSM6DSL_DataCut_InLine()
{
	SendData_UserDef.DataCode_Both[124] = (uint8_t)(IMU_UserDef.acc_x) & 0xff;	
	SendData_UserDef.DataCode_Both[125] = (uint8_t)(IMU_UserDef.acc_x) >>8 & 0xff;	
	SendData_UserDef.DataCode_Both[126] = (uint8_t)(IMU_UserDef.acc_y) & 0xff;	
	SendData_UserDef.DataCode_Both[127] = (uint8_t)(IMU_UserDef.acc_y) >>8 & 0xff;	
	SendData_UserDef.DataCode_Both[128] = (uint8_t)(IMU_UserDef.acc_z) & 0xff;	
	SendData_UserDef.DataCode_Both[129] = (uint8_t)(IMU_UserDef.acc_z) >>8 & 0xff;	
	SendData_UserDef.DataCode_Both[130] = (uint8_t)(IMU_UserDef.gyr_x) & 0xff;	
	SendData_UserDef.DataCode_Both[131] = (uint8_t)(IMU_UserDef.gyr_x) >>8 & 0xff;	
	SendData_UserDef.DataCode_Both[132] = (uint8_t)(IMU_UserDef.gyr_y) & 0xff;	
	SendData_UserDef.DataCode_Both[133] = (uint8_t)(IMU_UserDef.gyr_y) >>8 & 0xff;	
	SendData_UserDef.DataCode_Both[134] = (uint8_t)(IMU_UserDef.gyr_z) & 0xff;	
	SendData_UserDef.DataCode_Both[135] = (uint8_t)(IMU_UserDef.gyr_z) >>8 & 0xff;	
}

void LSM6DSL_DataCut_IMU()
{
	SendData_UserDef.DataCode_IMU[3]		= (uint8_t)(IMU_UserDef.acc_x) & 0xff;
	SendData_UserDef.DataCode_IMU[4]		= (uint8_t)(IMU_UserDef.acc_x) >>8 & 0xff;
	SendData_UserDef.DataCode_IMU[5]		= (uint8_t)(IMU_UserDef.acc_y) & 0xff;
	SendData_UserDef.DataCode_IMU[6]		= (uint8_t)(IMU_UserDef.acc_y) >>8 & 0xff;
	SendData_UserDef.DataCode_IMU[7]		= (uint8_t)(IMU_UserDef.acc_z) & 0xff;
	SendData_UserDef.DataCode_IMU[8]		= (uint8_t)(IMU_UserDef.acc_z) >>8 & 0xff;
	SendData_UserDef.DataCode_IMU[9]		= (uint8_t)(IMU_UserDef.gyr_x) & 0xff;
	SendData_UserDef.DataCode_IMU[10]		= (uint8_t)(IMU_UserDef.gyr_x) >>8 & 0xff;
	SendData_UserDef.DataCode_IMU[11]		= (uint8_t)(IMU_UserDef.gyr_y) & 0xff;
	SendData_UserDef.DataCode_IMU[12]		= (uint8_t)(IMU_UserDef.gyr_y) >>8 & 0xff;
	SendData_UserDef.DataCode_IMU[13]		= (uint8_t)(IMU_UserDef.gyr_z) & 0xff;
	SendData_UserDef.DataCode_IMU[14]		= (uint8_t)(IMU_UserDef.gyr_z) >>8 & 0xff;
}



#define			NEF_INIT_FLAG				0xff;
void LSM6DSL_Sleep()
{
	uint8_t result;
	result = NEF_INIT_FLAG;		
	/* �� CTRL4_C �Ĵ�����˯��ģʽʹ�ܣ�SLEEP��λ����Ϊ 1 ʱ��������ѡ������ODRΪ�Σ�����������˯��ģʽ*/
	while(result != NRF_SUCCESS)
	{
		result =	LSM6DSL_Write_Byte(LSM6DSL_CTRL1_XL, 0x01);//P60ҳ	 ���ٶ������µ�ģʽ
	}
	result = NEF_INIT_FLAG;	//��λ������
	while(result != NRF_SUCCESS)
	{
		result =	LSM6DSL_Write_Byte(LSM6DSL_CTRL2_G, 0x0C);//P61ҳ	 ���ٶ������µ�ģʽ
	}	
}

void LSM6DSL_WakeUp()
{
	uint8_t result;
	result = NEF_INIT_FLAG;		
	while(result != NRF_SUCCESS)
	{
		result = LSM6DSL_Write_Byte(LSM6DSL_CTRL1_XL, 0x31);//52hz���ٶ�	
	}
	result = NEF_INIT_FLAG;	//��λ������
	while(result != NRF_SUCCESS)
	{
		result = LSM6DSL_Write_Byte(LSM6DSL_CTRL2_G, 0x3C);//52hz���ٶ�
	}

}































