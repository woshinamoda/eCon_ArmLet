#include "myiic.h"


static void i2c_Delay(void)
{
	uint8_t i;
	for (i = 0; i < 20; i++);
}
//初始化IIC
void IIC_Init(void)
{			
	IIC_SCL(1);
	IIC_SDA(1);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA(1);	  	  
	IIC_SCL(1);
	i2c_Delay();
 	IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	i2c_Delay();
	IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL(0);
	IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	i2c_Delay();
	IIC_SCL(1); 
	IIC_SDA(1);//发送I2C总线结束信号
	i2c_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA(1);i2c_Delay();	   
	IIC_SCL(1);i2c_Delay();	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	i2c_Delay();
	IIC_SCL(1);
	i2c_Delay();
	IIC_SCL(0);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	i2c_Delay();
	IIC_SCL(1);
	i2c_Delay();
	IIC_SCL(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		i2c_Delay();   //对TEA5767这三个延时都是必须的
		IIC_SCL(1);
		i2c_Delay(); 
		IIC_SCL(0);	
		i2c_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL(0); 
        i2c_Delay();
		IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;   
		i2c_Delay(); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























