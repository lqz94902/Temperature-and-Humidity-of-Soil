#include <reg51.h>	         //���õ�Ƭ��ͷ�ļ�
#include "nrf24l01.h"
#define uchar unsigned char  //�޷����ַ��� �궨��	������Χ0~255
#define uint  unsigned int	 //�޷������� �궨��	������Χ0~65535
	
sbit led = P2^7;
sbit DQ  = P1^0;          //�¶ȴ������ӿ�
uint temp,shidu;
uchar cnt_300ms;

sbit  CS=P1^1;       
sbit CLK=P1^3;      
sbit DIO=P1^2;


unsigned int  ADC0832()
{
	unsigned char i,dat;
	CS=1;   //һ��ת�����ڿ�ʼ
	CLK=0;  //Ϊ��һ��������׼��
	CS=0;  //CS��0��Ƭѡ��Ч
	
	DIO=1;    //DIO��1���涨����ʼ�ź�  
	CLK=1;   //��һ������
	CLK=0;   //��һ��������½��أ���ǰDIO�����Ǹߵ�ƽ
	DIO=1;   //DIO��1�� ͨ��ѡ���ź�  
	CLK=1;   //�ڶ������壬��2��3�������³�֮ǰ��DI�������������λ��������ѡ��ͨ��������ѡͨ��CH0 
	CLK=0;   //�ڶ��������½��� 
	DIO=0;   //DI��0��ѡ��ͨ��0
	CLK=1;    //����������
	CLK=0;    //�����������½��� 
	DIO=1;    //�����������³�֮�������DIOʧȥ���ã�Ӧ��1
	CLK=1;    //���ĸ�����
	for(i=0;i<8;i++)  //��λ��ǰ
	{
		CLK=1;         //���ĸ�����
		CLK=0; 
		dat<<=1;       //�����洢��ĵ�λ����������
		dat|=(unsigned char)DIO; 	 //���������DIOͨ�������㴢����dat���λ 
	}	  		        
	CS=1;          //Ƭѡ��Ч 
	shidu=dat*1000/255;
	return shidu;	 //�����������ݷ���     
}

void delay_18B20(uint i)
{
	while(i--);
}

void Init_DS18B20() 
{
	 uchar x=0;
	 DQ=1;          //DQ��λ
	 delay_18B20(8);  //������ʱ
	 DQ=0;          //��Ƭ����DQ����
	 delay_18B20(80); //��ȷ��ʱ ���� 480us
	 DQ=1;          //��������
	 delay_18B20(14);
	 x=DQ;            //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	 delay_18B20(20);
}
/***********ds18b20��һ���ֽ�**************/  
uchar ReadOneChar()
{
	uchar i=0;
	uchar dat=0;
	for (i=8;i>0;i--)
	 {
		  DQ=0; // �������ź�
		  dat>>=1;
		  DQ=1; // �������ź�
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(4);
	 }
 	return(dat);
}
/*************ds18b20дһ���ֽ�****************/  
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
/**************��ȡds18b20��ǰ�¶�************/
void ReadTemperature()
{
	uchar a=0;
	uchar b=0;
	uchar t=0;
	Init_DS18B20();
	WriteOneChar(0xCC);    	// ����������кŵĲ���
	WriteOneChar(0x44); 	// �����¶�ת��
	delay_18B20(100);       // this message is very important
	Init_DS18B20();
	WriteOneChar(0xCC); 	//����������кŵĲ���
	WriteOneChar(0xBE); 	//��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
	delay_18B20(100);
	a=ReadOneChar();    	//��ȡ�¶�ֵ��λ
	b=ReadOneChar();   		//��ȡ�¶�ֵ��λ
	temp=((b*256+a)>>4);    //��ǰ�ɼ��¶�ֵ��16��ʵ���¶�ֵ
}


/***********************�����ʱ����*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/******************������**********************/	   
void main()
{	
	uchar j;
	
	TX_Mode();						//2.4G����ģ��IO�ڳ�ʼ��
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
				Tx_Buf[1] = ADC0832();        //ʪ��
				Tx_Buf[2] = temp;    					//�¶�
				Transmit(Tx_Buf);
				led = 0;
				delay_1ms(100);
				sta=SPI_Read(READ_REG +  STATUS);
				if(TX_DS)	 //��ǰSTATUS״̬  �����ж�Ӧʹbit5 = 1 
				{
					SPI_RW_Reg(WRITE_REG + STATUS,sta); 
				}
				if(MAX_RT)  //����Ƿ��ͳ�ʱ 
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