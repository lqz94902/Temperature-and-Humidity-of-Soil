#include <reg51.h>	         //调用单片机头文件
#include "nrf24l01.h"
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535
	
sbit led = P2^7;
sbit DQ  = P1^0;          //温度传感器接口
uint temp,shidu;
uchar cnt_300ms;

sbit  CS=P1^1;       
sbit CLK=P1^3;      
sbit DIO=P1^2;


unsigned int  ADC0832()
{
	unsigned char i,dat;
	CS=1;   //一个转换周期开始
	CLK=0;  //为第一个脉冲作准备
	CS=0;  //CS置0，片选有效
	
	DIO=1;    //DIO置1，规定的起始信号  
	CLK=1;   //第一个脉冲
	CLK=0;   //第一个脉冲的下降沿，此前DIO必须是高电平
	DIO=1;   //DIO置1， 通道选择信号  
	CLK=1;   //第二个脉冲，第2、3个脉冲下沉之前，DI必须跟别输入两位数据用于选择通道，这里选通道CH0 
	CLK=0;   //第二个脉冲下降沿 
	DIO=0;   //DI置0，选择通道0
	CLK=1;    //第三个脉冲
	CLK=0;    //第三个脉冲下降沿 
	DIO=1;    //第三个脉冲下沉之后，输入端DIO失去作用，应置1
	CLK=1;    //第四个脉冲
	for(i=0;i<8;i++)  //高位在前
	{
		CLK=1;         //第四个脉冲
		CLK=0; 
		dat<<=1;       //将下面储存的低位数据向右移
		dat|=(unsigned char)DIO; 	 //将输出数据DIO通过或运算储存在dat最低位 
	}	  		        
	CS=1;          //片选无效 
	shidu=dat*1000/255;
	return shidu;	 //将读出的数据返回     
}

void delay_18B20(uint i)
{
	while(i--);
}

void Init_DS18B20() 
{
	 uchar x=0;
	 DQ=1;          //DQ复位
	 delay_18B20(8);  //稍做延时
	 DQ=0;          //单片机将DQ拉低
	 delay_18B20(80); //精确延时 大于 480us
	 DQ=1;          //拉高总线
	 delay_18B20(14);
	 x=DQ;            //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	 delay_18B20(20);
}
/***********ds18b20读一个字节**************/  
uchar ReadOneChar()
{
	uchar i=0;
	uchar dat=0;
	for (i=8;i>0;i--)
	 {
		  DQ=0; // 给脉冲信号
		  dat>>=1;
		  DQ=1; // 给脉冲信号
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(4);
	 }
 	return(dat);
}
/*************ds18b20写一个字节****************/  
void WriteOneChar(uchar dat)
{
 	uchar i=0;
 	for (i=8;i>0;i--)
 	{
  		DQ=0;
 		DQ=dat&0x01;
    	delay_18B20(5);
 		DQ=1;
    	dat>>=1;
	}
}
/**************读取ds18b20当前温度************/
void ReadTemperature()
{
	uchar a=0;
	uchar b=0;
	uchar t=0;
	Init_DS18B20();
	WriteOneChar(0xCC);    	// 跳过读序号列号的操作
	WriteOneChar(0x44); 	// 启动温度转换
	delay_18B20(100);       // this message is very important
	Init_DS18B20();
	WriteOneChar(0xCC); 	//跳过读序号列号的操作
	WriteOneChar(0xBE); 	//读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	delay_18B20(100);
	a=ReadOneChar();    	//读取温度值低位
	b=ReadOneChar();   		//读取温度值高位
	temp=((b*256+a)>>4);    //当前采集温度值除16得实际温度值
}


/***********************软件延时函数*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/******************主程序**********************/	   
void main()
{	
	uchar j;
	
	TX_Mode();						//2.4G无线模块IO口初始化
	delay_1ms(650);
	for(j=0;j<80;j++)
	ReadTemperature();
	TMOD=1;
	TH0=0XEC;
	TL0=0X78;
	TR0=1;
	while(1)
	{
		while(!TF0);
		{
			TF0=0;
			TH0=0XEC;
			TL0=0X78;
			cnt_300ms++;
			if(cnt_300ms>=60)
			{
				ReadTemperature();
				Tx_Buf[0] = 0;  
				Tx_Buf[1] = ADC0832();        //湿度
				Tx_Buf[2] = temp;    					//温度
				Transmit(Tx_Buf);
				led = 0;
				delay_1ms(100);
				sta=SPI_Read(READ_REG +  STATUS);
				if(TX_DS)	 //当前STATUS状态  发送中断应使bit5 = 1 
				{
					SPI_RW_Reg(WRITE_REG + STATUS,sta); 
				}
				if(MAX_RT)  //如果是发送超时 
				{
					SPI_RW_Reg(WRITE_REG + STATUS,sta);
				}
				led = 1;
				delay_1ms(100);
				cnt_300ms=0;
			}
		}
	}
}