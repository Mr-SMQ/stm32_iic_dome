/****************************************************************************
* 版权信息：
*
* 系统名称：
*
* 文件名称：iic_com.c
*
* 文件说明: 
*	 
*		  
* 其他说明：
*
* 作	者：rony
*
* 版本信息：V1.00
*
* 设计日期：2018-12-17
*
***************************************************************************/
#include "global.h"
#include "iic_com.h"


//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     	// sda线输出
	IIC_SDA = 1;	  	  
	IIC_SCL = 1;
	delay_us(4);
 	IIC_SDA = 0;	// START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL = 0;	// 钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();		// sda线输出
	IIC_SCL = 0;
	IIC_SDA = 0;	// STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL = 1; 
	IIC_SDA = 1;	// 发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      // SDA设置为输入  
	IIC_SDA = 1;
	delay_us(1);	   
	IIC_SCL = 1;
	delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
}

//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}		


//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			IIC_SDA = 1;
		else
			IIC_SDA = 0;
		txd <<= 1; 	  
		delay_us(2);  
		IIC_SCL = 1;
		delay_us(2); 
		IIC_SCL = 0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL = 0; 
        delay_us(2);
		IIC_SCL = 1;
        receive <<= 1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


