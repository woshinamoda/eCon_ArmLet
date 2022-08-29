#include "adc_data.h"
#include "adc.h"

extern uint8_t htim10_flag;
uint32_t AD_Value_Temp[9]={0};
uint8_t Emg_Data_Sent[20]={0x55,0xAA};     	  //Emg_Data_Sent

uint8_t AD_Sent[20]={0};

uint32_t Li_battery=0;		//﮵�ص���ADC


void ADCStart(void)
{
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&AD_Value,9);//ADCѭ��ģʽ����
}


//CRC8У�� 
//ptr:ҪУ������� 
//len:���鳤�� 
//����ֵ:CRC8�� 
uint8_t Get_Crc8(uint8_t *ptr,uint16_t len) 
{ 
    uint8_t crc; 
    uint8_t i; 
    crc=0; 
    while(len--) 
    { 
        crc^=*ptr++; 
        for(i=0;i<8;i++) 
        { 
            if(crc&0x01)
							crc=(crc>>1)^0x8C; 
            else 
							crc >>= 1; 
        } 
    } 
    return crc; 
} 

uint8_t crcCode(uint8_t *ptr,uint16_t len)
{
	char crc=0;
	char crcCode;
	for(int i=2;i<len;i++)
	{
		crc+=ptr[i];
	}
	crcCode=crc%256;
	return crcCode;
}



void get_ADCdata(void)
{
	uint8_t temp;
	for(int j=0;j<8;j++)
	{
			AD_Value_Temp[j]=AD_Value[j];									//���ȶ�ȡһ�����ݣ���ֹ��ˢ�µ�
	}
	Li_battery=AD_Value[8];														//��ȡ����ص���
	
	Emg_Data_Sent[0]=0x55;								
	Emg_Data_Sent[1]=0xAA;														//ͷ��
	
	for (int n=0;n<4;n++)															//1.5�ֽ�(12λ) x 8 = 12�ֽ�
	{
	   AD_Sent[3*n]=AD_Value_Temp[2*n]>>4;

	   AD_Sent[3*n+1]=(AD_Value_Temp[2*n]&0x000f);		
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]<<4;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]&0xf0;
		 temp=(AD_Value_Temp[2*n+1]&0x0f00)>>8;
		 temp=temp&0x0f;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]|temp;
		 AD_Sent[3*n+2]=AD_Value_Temp[2*n+1]&0x00ff;
	}

	Emg_Data_Sent[14] = 0;															//ÿ��У��ǰˢ����0
	for(int i=2;i<14;i++)
	{
			Emg_Data_Sent[i]=AD_Sent[i-2];
		  Emg_Data_Sent[14] += Emg_Data_Sent[i];					//15�Ƕ�Ӧ���ݺ����λ
			/* ����汾���У�飬����ʹ���Ǹ���matlab����  */		
	}	
	
	//Emg_Data_Sent[14]=crcCode(Emg_Data_Sent,14); 
	/* ��ΰ���汾 crcCodeУ�� */	
	
	Emg_Data_Sent[15] = 0;															//trigger����Ԫ�أ����忴ʹ�����
}


void test_adc_Data(void)
{
	uint8_t temp;
	number++;
	for(int j=0;j<8;j++)
	{
			AD_Value_Temp[j]=AD_Value[j];									//���ȶ�ȡһ�����ݣ���ֹ��ˢ�µ�
	}
	Li_battery=AD_Value[8];														//��ȡ����ص���
	
	Emg_Data_Sent[0]=0xBB;								
	Emg_Data_Sent[1]=0xAA;														
	Emg_Data_Sent[2]=12;													
	
	for (int n=0;n<4;n++)															//1.5�ֽ�(12λ) x 8 = 12�ֽ�
	{
	   AD_Sent[3*n]=AD_Value_Temp[2*n]>>4;

	   AD_Sent[3*n+1]=(AD_Value_Temp[2*n]&0x000f);		
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]<<4;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]&0xf0;
		 temp=(AD_Value_Temp[2*n+1]&0x0f00)>>8;
		 temp=temp&0x0f;
		 AD_Sent[3*n+1]=AD_Sent[3*n+1]|temp;
		 AD_Sent[3*n+2]=AD_Value_Temp[2*n+1]&0x00ff;
	}

	Emg_Data_Sent[15] = 0;															//ÿ��У��ǰˢ����0
	for(int i=3;i<15;i++)
	{
			Emg_Data_Sent[i]=AD_Sent[i-3];
		  Emg_Data_Sent[15] += Emg_Data_Sent[i];					//15�Ƕ�Ӧ���ݺ����λ
			/* ����汾���У�飬����ʹ���Ǹ���matlab����  */		
	}	
	
	//Emg_Data_Sent[14]=crcCode(Emg_Data_Sent,14); 
	/* ��ΰ���汾 crcCodeУ�� */	
	
	Emg_Data_Sent[16] = number;															//trigger����Ԫ�أ����忴ʹ�����
}





void get_ADCdata_Double_for_Onechannel(void)
{
	uint8_t temp;
	/* �ȶ�ȡ����ֹ��ˢ��*/
	for(int j=0;j<8;j++)
	{
			AD_Value_Temp[j]=AD_Value[j];
	}
	/* read battery value */
	Li_battery=AD_Value[8];


	Emg_Data_Sent[0] = 0x55;
	Emg_Data_Sent[1] = 0xaa;

	Emg_Data_Sent[2] = (uint32_t)AD_Value_Temp[0]>>8;
	Emg_Data_Sent[3] = (uint32_t)AD_Value_Temp[0]&0x00ff;
	
	Emg_Data_Sent[4] = (uint32_t)AD_Value_Temp[1]>>8;
	Emg_Data_Sent[5] = (uint32_t)AD_Value_Temp[1]&0x00ff;

	Emg_Data_Sent[6] = (uint32_t)AD_Value_Temp[2]>>8;
	Emg_Data_Sent[7] = (uint32_t)AD_Value_Temp[2]&0x00ff;

	Emg_Data_Sent[8] = (uint32_t)AD_Value_Temp[3]>>8;
	Emg_Data_Sent[9] = (uint32_t)AD_Value_Temp[3]&0x00ff;

	Emg_Data_Sent[10] = (uint32_t)AD_Value_Temp[4]>>8;
	Emg_Data_Sent[11] = (uint32_t)AD_Value_Temp[4]&0x00ff;

	Emg_Data_Sent[12] = (uint32_t)AD_Value_Temp[5]>>8;
	Emg_Data_Sent[13] = (uint32_t)AD_Value_Temp[5]&0x00ff;

	Emg_Data_Sent[14] = (uint32_t)AD_Value_Temp[6]>>8;
	Emg_Data_Sent[15] = (uint32_t)AD_Value_Temp[6]&0x00ff;

	Emg_Data_Sent[16] = (uint32_t)AD_Value_Temp[7]>>8;
	Emg_Data_Sent[17] = (uint32_t)AD_Value_Temp[7]&0x00ff;
	
	

}


























