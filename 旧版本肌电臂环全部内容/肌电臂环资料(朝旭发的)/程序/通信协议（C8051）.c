
#include "Define.h"

extern unsigned char  	g_ADresult[16];
extern unsigned char	g_check;
						
extern unsigned char	Varible_L;
extern unsigned char	Varible_H;													  	
/********************************************/
/* ���ƣ�Start_AD							*/
/* ˵����ִ��ADת��			  		        */ 
/********************************************/
void Start_AD(void)
{
	Reset = TRUE;
	Reset = TRUE;							// ��λADоƬ
	Reset = FALSE;
	
	Convst_x = FALSE;
	Convst_x = FALSE;			
	Convst_x = TRUE;						// ����ת��

	while(Busy == TRUE);

}

/********************************************/
/* ���ƣ�Get_AD_Result						*/
/* ˵������ȡADת�����		  		        */ 
/********************************************/
bit Get_AD_Result(void)
{
	unsigned char j = 0;

 	Cs = FALSE;
	while(j<16)
	{
		Sclk = FALSE;
		Sclk = FALSE;
		g_ADresult[j]= P2;
		Sclk = TRUE;
		j++;																 
	}
	Cs = TRUE;
/*
	//g_ADresult[6]=0; 
	//g_ADresult[7]=0;

	//test
	Varible_L++;
	if(Varible_L == 0)Varible_H++;
	g_ADresult[2]=Varible_H; 
	g_ADresult[3] = Varible_L;
		
	g_ADresult[0]=0; 
	g_ADresult[1] = 0;
	g_ADresult[4]=0; 
	g_ADresult[5] = 0;
	g_ADresult[6]=0; 
	g_ADresult[7] = 0; 
	g_ADresult[8]=0; 
	g_ADresult[9] = 0;
	g_ADresult[10]=0; 
	g_ADresult[11] = 0;
	g_ADresult[12]=0; 
	g_ADresult[13] = 0;
	g_ADresult[14]=0; 
	g_ADresult[15] = 0;

/* ˵�����ƶ�ͨ��Э�飬���������8ͨ����ģ�������߰�λ��Ͱ�λ�������ݰ�ͷ��55AA����β��У��λ	 	        */ 
/********************************************/

*/	
	g_check=0;
	SBUF0 =0x55;while(TI0 == 0);TI0 = 0;
	SBUF0 =0xAA;while(TI0 == 0);TI0 = 0;
	for(j=0;j<16;j++)
	{
		g_check+=g_ADresult[j];
		SBUF0 =g_ADresult[j];while(TI0 == 0);TI0 = 0;
	}	 
	SBUF0 =g_check;while(TI0 == 0);TI0 = 0;
	return TRUE;								
}

/********************************************/
/* ���ƣ�Set_OS								*/
/* ˵����������ģʽ����		  		        */ 
/********************************************/
void Set_OS(unsigned char times)
{
	switch(times)
	{
		case 0:
		{
			OS2 = 0;
			OS1 = 0;
			OS0 = 0;
			break;
		}
		case 2:
		{
			OS2 = 0;
			OS1 = 0;
			OS0 = 1;
			break;	
		}
		case 4:
		{
			OS2 = 0;
			OS1 = 1;
			OS0 = 0;	
		}
		case 8:
		{
			OS2 = 0;
			OS1 = 1;
			OS0 = 1;
			break;	
		}
		case 16:
		{
			OS2 = 1;
			OS1 = 0;
			OS0 = 0;
			break;	
		}
		case 32:
		{
			OS2 = 1;
			OS1 = 0;
			OS0 = 1;
			break;	
		}
		case 64:
		{
			OS2 = 1;
			OS1 = 1;
			OS0 = 0;
			break;	
		}		
		default: break;
	}
}