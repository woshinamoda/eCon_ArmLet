
#include "Define.h"

extern unsigned char  	g_ADresult[16];
extern unsigned char	g_check;
						
extern unsigned char	Varible_L;
extern unsigned char	Varible_H;													  	
/********************************************/
/* 名称：Start_AD							*/
/* 说明：执行AD转换			  		        */ 
/********************************************/
void Start_AD(void)
{
	Reset = TRUE;
	Reset = TRUE;							// 复位AD芯片
	Reset = FALSE;
	
	Convst_x = FALSE;
	Convst_x = FALSE;			
	Convst_x = TRUE;						// 启动转换

	while(Busy == TRUE);

}

/********************************************/
/* 名称：Get_AD_Result						*/
/* 说明：获取AD转换结果		  		        */ 
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

/* 说明：制定通信协议，例程是针对8通道的模拟量（高八位与低八位），数据包头是55AA，结尾有校验位	 	        */ 
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
/* 名称：Set_OS								*/
/* 说明：过采样模式设置		  		        */ 
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