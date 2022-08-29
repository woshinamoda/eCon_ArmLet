
#include "stm32f4xx.h"
#include "hardware_init.h"
#include "bsp_led.h"
#include "stm_ADC.h"
#define Li_BAT2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)
u8 Uart1_Data = 0;
u8 Uart1_Flag = 0;
u8 Low_Power_Flag = 0;
u8 Wkup_Flag = 0;
u8 LED_Flag = 0;
u8 Motor_Flag = 0;
u32 timeMotor[10]={0};
int flagTimeMotor=-1;
u16 Wkup_cont = 0;
u16 Wkup_store = 0;
u16 Li_battery = 0;
u16 Low_Power_Cont = 0;
u32 Bat_Full_Cnt=0;   //the cnt of virtual full state
u32 full_cnt=0;

char *code="sjtuecon";//code
u8 lenCode=8;//length of code
u8 indCode=0;//indCode
u8 flagCode=0;//the matching status of code

///////////////////////////////////////////////////
//here is the sending frequency
u16 mpuperiod = 50;  //frequency of mpu   设置9轴采样率
u16 period = 1000;   //frequency of EMG  设置肌电采样率
u32 full_time=8;   //set the time(minutes)after the ADC of batery reach the virtual full state 判断电池充满时间
u32 wait_time=10; //wait time(min) for action     设置静置自动休眠时间
u32 uart_baud=460800; //buad of uart115200 work 460800  设置单片机串口波特率
u32 cntMotor=0;
//////////////////////////////////////////////////

bool flag=false;//clock flag 
bool mpu_flag=false;//mpuflag
 
extern signed short int init_ax, init_ay, init_az;
extern signed short int init_gx, init_gy, init_gz;
signed short int old_ax=0;

void TIM3_Configuration(void)    
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //???TIM3??
      
  TIM_TimeBaseInitStructure.TIM_Period = 100000/period; //??????
	TIM_TimeBaseInitStructure.TIM_Prescaler=999;  //?????
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// ???????TIM3

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //??????3????

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //?????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// ????NVIC
 
	TIM_Cmd(TIM3,ENABLE); //??????3
}  
 void TIM4_Configuration(void)    
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //???TIM3??
      
  TIM_TimeBaseInitStructure.TIM_Period = 100000/mpuperiod;; //2000*1000/100Mhz=time
	TIM_TimeBaseInitStructure.TIM_Prescaler=999;  //?????
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);// ???????TIM3

	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //??????3????

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //?????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// ????NVIC
 
	TIM_Cmd(TIM4,ENABLE); //??????3
}  

void ioconfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//LZX//设置中断优先级分组
	full_cnt=full_time*period*60;
	int count=0;
	int flip_loop=100;
	short tempDiff;
	delay_init(); //延时初始化、系统时钟检测
	Motor_Init();
	Motor_Long_Strong();  //电机2S  强振
	HardWare_Init();
	bsp_InitUart(COM1,uart_baud);
	TIM3_Configuration();
	TIM4_Configuration();
	enum col cur_col=red;

  while(1)
  {
		if(mpu_flag)
		{
			send_mpu();
			mpu_flag=false;
			tempDiff=absd(Wkup_store,init_gx);
			if(tempDiff<500) Wkup_cont+=1;
			else 
			{
				Wkup_cont=Wkup_Flag=0;
			}
			
			Wkup_store = init_gx;
			old_ax=init_ax;
			if(Wkup_cont>mpuperiod*60*wait_time) Wkup_Flag=1;      //静置10min  后待机
//						if(Wkup_cont>mpuperiod*15) Wkup_Flag=1;      //静置10min  后待机
			
			if(Wkup_Flag)
			{
				Wkup_Flag=0;
				LED1_ON();
				LED2_ON();
				LED3_ON();
				Motor_Short_Soft();  //电机1S  轻振
				Sys_Enter_Standby();
			}
		}
		if(!flag) continue;
		if(flagTimeMotor>=0)
		{
			if(flagTimeMotor%2) Motor_OFF();
			else Motor_ON();
			timeMotor[flagTimeMotor]--;
			if(timeMotor[flagTimeMotor]==0) flagTimeMotor--;
		}
		else Motor_OFF();
		flag=false;
		count++;
//    get_ADCdata();                    //读取ADC  数据

	  
	  if(Li_battery<2296) Low_Power_Cont+=1; //1.85/3.3*4096 2296
		else Low_Power_Cont=0;
	  if(Low_Power_Cont>600) Low_Power_Flag = 1;
	  
		if(Li_BAT2)
		{
			//电源充满
			if(Li_battery>2610) //2.1/3.3*4096             //USB  充电提示
	  	{
				//在虚满状态下，计时到指定时间后再将灯翻转
				if(full_cnt>Bat_Full_Cnt) 
				{
					Bat_Full_Cnt++;
					if(count%flip_loop==0) LED_TOGGLE(green);//绿色闪烁
				}
				else
				{
					LED_green();
					Low_Power_Cont = 0;
					Low_Power_Flag = 0;
				}
	  	}
			else
			{
					//未充满时虚满计数置零
					Bat_Full_Cnt=0;
					//未充满且充电状态
					if(count%flip_loop==0)LED_TOGGLE(green);//绿色闪烁
			}
		}
		else 
		{
				if(Low_Power_Flag)//如果缺电红色闪烁
				{
					if(count%flip_loop==0)LED_TOGGLE(red);//红色闪烁
				}
				else LED_blue();				//不在低电压状态，长蓝
		}
	  if(Motor_Flag)
	  {
	  	  Motor_Flag=0;
	  	  //Motor_Short_Strong();
	  }  
  }
}

void USART1_IRQHandler(void)
{
	//接收中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Uart1_Data = USART_ReceiveData(USART1);

		if(Uart1_Data<6) flagTimeMotor=MotorSetType(Uart1_Data,timeMotor);
		else 
		{
			flagTimeMotor=0;
			timeMotor[0]=5*Uart1_Data;
		}
		Uart1_Flag = 1;
	}
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(USART1);    //读DR
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //????
	{
		flag=true;
		get_ADCdata();
		if(if_imu_sent_flag)
		{
			if_imu_sent_flag = 0;
			USART_SendArray(USART1,Imu_Data_Sent,21);	
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //???????
}
void TIM4_IRQHandler(void)  
{   
  if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //????
  {
				mpu_flag=true;
  }
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //???????
}  