#include "nrf_ads1299.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "driver_power.h"

#define SPI_INSTANCE  1 /**< SPI instance index. */


static volatile bool spi_xfer_done;  //SPI���ݴ�����ɱ�־
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /* < SPI instance. */

static uint8_t    spi_tx_buf[256];   /**< TX buffer. */
static uint8_t    spi_rx_buf[256];   /**< RX buffer. */

uint8_t			ADS1299_ID;
uint8_t			regData[24]; 	

void ADS1292_gpio_config()
{//����ADS1292��gpio��ʼ��	
	nrf_gpio_cfg_output(SPI_SS_PIN);
	nrf_gpio_cfg_output(ADS_START);
	nrf_gpio_cfg_output(ADS_RESET);
	nrf_gpio_cfg_output(ADS_PWDN);
	
	CS_H;			//CS�ߵ�ƽ
	PWDN_H;		//PWDN�ߵ�ƽ
	RESET_H;	//RESET�ߵ�ƽ
	START_L;	//START�͵�ƽ
}




void spi_event_handler(nrf_drv_spi_evt_t const * p_event,	void *	p_context)
{/*	SPI�ж��¼�������	*/
	//����SPI�������  
	spi_xfer_done = true;
}


void SPI_User_init()
{/*	����SPI�����ֹ���ģʽ
	
		ģʽ				����
	ģʽ0			CPOL=0			CPHA=0
	ģʽ1			CPOL=0			CPHA=1
	ģʽ2			CPOL=1			CPHA=0
	ģʽ3			CPOL=1			CPHA=1	
	*/
	//���ö���spi��ʼ���ṹ�� = Ĭ������
	nrf_drv_spi_config_t	spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
	//��дĬ�����������������
	spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;									//�û��Զ��壬�����������
	spi_config.mosi_pin = SPI_MOSI_PIN;
	spi_config.miso_pin =	SPI_MISO_PIN;
	spi_config.sck_pin = 	SPI_SCK_PIN;
	
	spi_config.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY;		//�ж����ȼ�������ں궨���������õ���6
	spi_config.frequency = NRF_DRV_SPI_FREQ_8M;										//spiƵ������8M
	spi_config.orc = 0xFF;																				//ORC�Ĵ�������0xff
	spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;				//���ݴ����λ��ǰ
	spi_config.mode = NRF_DRV_SPI_MODE_1;													//ģʽ1
	
	//��ʼ��SPI
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));	
}


void SPI_uninit()
{
	nrf_drv_spi_uninit(&spi);
}



uint8_t	ADS_xfer(uint8_t byte)
{//SPI��ָ����ַ�϶�ȡһ������
	//* nrf52840 SDK�Ķ�д�Ƿ�װ��ͬһ���������	*/
	uint8_t d_read;						

	//8λ��ַ
	spi_tx_buf[0] =	 byte;		

//	������ɱ�־����Ϊfalse
//	spi_xfer_done = false;
	
	//�������ݴ���
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spi_tx_buf, 1, spi_rx_buf, 1));	
	
//	�ȴ�SPI�������
//	while(!spi_xfer_done);

	d_read = spi_rx_buf[0];
			
	//���ض�ȡ��ֵ
	return	d_read;	
}



uint8_t	ADS_WREG(uint8_t	address,	uint8_t	cmd)
{//ADS1299д�Ĵ���
	uint8_t	Opcode1 = address + 0x40;						//����д��Ĵ�����ַ

	CS_L;																				//����CS
	ADS_xfer(Opcode1);  												//��һ�������룺�Ĵ���address
	for(int j=250;j>0;j--){};										//4 Tclk delay
	ADS_xfer(0x00);															//���������� 0x01 - 1 = 0x00
	for(int j=250;j>0;j--){};										//4 Tclk delay
	ADS_xfer(cmd); 															//д��Ĵ���ֵ
	for(int j=250;j>0;j--){};										//4 Tclk delay
	CS_H;																				//CS�øߣ�ʹ������

	return regData[address]; 
}




uint8_t ADS_RREG(uint8_t address)
{//ADS1299���Ĵ���
	uint8_t	Opcode1 = address + 0x20;						//�����ȡ�Ĵ�����ַ

	CS_L;																				//����CS
	ADS_xfer(Opcode1);  												//��һ�������룺�Ĵ���address
	for(int j=250;j>0;j--){};										//4 Tclk delay
	ADS_xfer(0x00);															//���������� 0x01 - 1 = 0x00
	for(int j=250;j>0;j--){};										//4 Tclk delay
	regData[address] = ADS_xfer(0x00); 					// update mirror location with returned byte
	for(int j=250;j>0;j--){};										//4 Tclk delay
	CS_H;	

	return regData[address]; 										// return requested register value
}


void	ADS1299_RESET(void)
{//ADS1299��λ

	CS_L;
	ADS_xfer(_RESET);
	nrf_delay_ms(1);
	CS_H;	

}


void ADS_SDATAC(void)
{//ADS1299ֹͣ������ȡģʽ

	CS_L;
  ADS_xfer(_SDATAC);
  for(int j=250;j>0;j--){};										//4 Tclk delay
	CS_H;	

  nrf_delay_ms(1); 														//must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}


uint8_t	ADS_getDeviceID(void)
{//ADS��ȡоƬID
  ADS1299_ID = ADS_RREG(ID_REG);
  return ADS1299_ID;
}

void  ADS1299_START(void)
{//ADS�����ź�

	CS_L;
  ADS_xfer(_START); 												// KEEP ON-BOARD AND ON-DAISY IN SYNC
  for(int j=250;j>0;j--){};									//4 Tclk delay
	CS_H;
}

void    ADS_RDATAC(void)
{//ADS������ȡģʽ

	CS_L;
  ADS_xfer(_RDATAC); 									// read data continuous
  for(int j=250;j>0;j--){};						//4 Tclk delay
	CS_H;

  nrf_delay_ms(1);
}





void		ADS_Enter_StandBY()
{//ADS����StandBYģʽ
	CS_L;
  ADS_xfer(_STANDBY); 								// read data continuous
  for(int j=250;j>0;j--){};						//4 Tclk delay
	CS_H;
  nrf_delay_ms(1);
}



void		ADS_Exit_StandBY()
{//ADS�Ƴ�StandBYģʽ
	CS_L;
  ADS_xfer(_WAKEUP); 								//read data continuous
  for(int j=250;j>0;j--){};					//4 Tclk delay
	CS_H;
  nrf_delay_ms(1);
}

void		ADS1299_Sleep_mode()
{//ADS1299����˯��ģʽ����͹���
	PWDN_L;														//����PWDN���ţ�ʹ��оƬ�ڲ����ڶϵ�ģʽ
	Control_AVDD(0);									//�ر�AVDD����
}


void initialize_ads(void)
{
	
	nrf_delay_ms(500);	// recommended power up sequence requiers >Tpor (2��18��Tclk) pdf.70

	ADS1299_RESET();
	nrf_delay_ms(500);	// recommended to wait 18 Tclk before using device

	ADS_SDATAC();
	nrf_delay_ms(500);

//	ADS_getDeviceID();
//	nrf_delay_ms(1000);
	//reg setting

//�ġ�Impedance-31.2Hz-AC-6nA
#if 0

	ADS_WREG(CONFIG1,0x95);//500Hz
	nrf_delay_ms(100);
	ADS_WREG(CONFIG2,0xC0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG3,0xEC);
	nrf_delay_ms(100);
	ADS_WREG(LOFF,0x02);
	nrf_delay_ms(100);


	ADS_WREG(BIAS_SENSP,0xFF);
	nrf_delay_ms(100);
	ADS_WREG(BIAS_SENSN,0xFF);
	nrf_delay_ms(100);
	ADS_WREG(LOFF_SENSP,0xFF);
	nrf_delay_ms(100);
	ADS_WREG(LOFF_SENSN,0x00);
	nrf_delay_ms(100);
  ADS_WREG(LOFF_FLIP,0x00);
	nrf_delay_ms(100);
	ADS_WREG(MISC1,0x20);//����SRB1
	nrf_delay_ms(100);
	
	ADS_WREG(CH1SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH2SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH3SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH4SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH5SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH6SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH7SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH8SET,0x60);
	nrf_delay_ms(10);
	
	
	ADS1299_START();
	nrf_delay_ms(100);
	ADS_RDATAC();
	nrf_delay_ms(100);
	
#endif
	
//һ��normal
#if 1

	ADS_WREG(CONFIG1,0x96);//250Hz
	//ADS_WREG(CONFIG1,0x95);//500Hz	
	nrf_delay_ms(100);
	ADS_WREG(CONFIG2,0xC0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG3,0xEC);
	nrf_delay_ms(100);

	ADS_WREG(BIAS_SENSP,0xFF);
	nrf_delay_ms(100);
	ADS_WREG(BIAS_SENSN,0xFF);
	nrf_delay_ms(100);
	ADS_WREG(MISC1,0x20);//����SRB1
	nrf_delay_ms(100);
	
	ADS_WREG(CH1SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH2SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH3SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH4SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH5SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH6SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH7SET,0x60);
	nrf_delay_ms(10);
	ADS_WREG(CH8SET,0x60);
	nrf_delay_ms(10);	
	
	ADS1299_START();
	nrf_delay_ms(100);
	ADS_RDATAC();
	nrf_delay_ms(100);
	
#endif

	
// ����internal short noise
#if  0
	ADS_WREG(CONFIG3,0xE0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG1,0x95);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG2,0xC0);
	nrf_delay_ms(100);
	ADS_WREG(CH1SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH2SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH3SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH4SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH5SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH6SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH7SET,0x01);
	nrf_delay_ms(10);
	ADS_WREG(CH8SET,0x01);
	nrf_delay_ms(10);

	ADS_START();
	nrf_delay_ms(100);
	ADS_RDATAC();
	nrf_delay_ms(100);
#endif


// ����internal test wave
#if 0
	ADS_WREG(CONFIG3,0xE0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG1,0x95);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG2,0xD0);
	nrf_delay_ms(100);
	ADS_WREG(CH1SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH2SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH3SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH4SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH5SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH6SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH7SET,0x05);
	nrf_delay_ms(10);
	ADS_WREG(CH8SET,0x05);
	nrf_delay_ms(10);
	
	ADS1299_START();
	nrf_delay_ms(100);	
	ADS_RDATAC();
	nrf_delay_ms(100);
#endif	
	
}


void updateBoardData(void)
{
	uint8_t inByte;
	int byteCounter = 0;
	CS_L;
  for (int i = 0; i < 3; i++)
  {
    inByte = ADS_xfer(0x00); //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    ECG_UserDef.boardStat = (ECG_UserDef.boardStat << 8) | inByte;
  }
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 3; j++)
    { 
			inByte = ADS_xfer(0x00);
      byteCounter++;
      ECG_UserDef.boardChannelDataInt[i] = (ECG_UserDef.boardChannelDataInt[i] << 8) | inByte; // int data goes here
    }
  }	
	CS_H;
}

void	ADS1299_DataCut_InLine()
{
	updateBoardData();
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+3] = (uint8_t)(ECG_UserDef.boardChannelDataInt[0] & 0xff);			//��1ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+4] = (uint8_t)(ECG_UserDef.boardChannelDataInt[0]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+5] = (uint8_t)(ECG_UserDef.boardChannelDataInt[0]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+6] = (uint8_t)(ECG_UserDef.boardChannelDataInt[1] & 0xff);			//��2ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+7] = (uint8_t)(ECG_UserDef.boardChannelDataInt[1]>>8 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+8] = (uint8_t)(ECG_UserDef.boardChannelDataInt[1]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+9] = (uint8_t)(ECG_UserDef.boardChannelDataInt[2] & 0xff);			//��3ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+10] = (uint8_t)(ECG_UserDef.boardChannelDataInt[2]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+11] = (uint8_t)(ECG_UserDef.boardChannelDataInt[2]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+12] = (uint8_t)(ECG_UserDef.boardChannelDataInt[3] & 0xff);			//��4ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+13] = (uint8_t)(ECG_UserDef.boardChannelDataInt[3]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+14] = (uint8_t)(ECG_UserDef.boardChannelDataInt[3]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+15] = (uint8_t)(ECG_UserDef.boardChannelDataInt[4] & 0xff);			//��5ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+16] = (uint8_t)(ECG_UserDef.boardChannelDataInt[4]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+17] = (uint8_t)(ECG_UserDef.boardChannelDataInt[4]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+18] = (uint8_t)(ECG_UserDef.boardChannelDataInt[5] & 0xff);			//��6ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+19] = (uint8_t)(ECG_UserDef.boardChannelDataInt[5]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+20] = (uint8_t)(ECG_UserDef.boardChannelDataInt[5]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+21] = (uint8_t)(ECG_UserDef.boardChannelDataInt[6] & 0xff);			//��7ͨ��
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+22] = (uint8_t)(ECG_UserDef.boardChannelDataInt[6]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+23] = (uint8_t)(ECG_UserDef.boardChannelDataInt[6]>>16 & 0xff);		
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+24] = (uint8_t)(ECG_UserDef.boardChannelDataInt[7] & 0xff);			//��8ͨ��	
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+25] = (uint8_t)(ECG_UserDef.boardChannelDataInt[7]>>8 & 0xff);
	SendData_UserDef.DataCode_ECG[SendData_UserDef.ECG_index*24+26] = (uint8_t)(ECG_UserDef.boardChannelDataInt[7]>>16 & 0xff);	
}








































































































































