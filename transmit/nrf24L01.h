#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#define uchar unsigned char  //�޷����ַ��� �궨��	������Χ0~255
#define uint  unsigned int	 //�޷������� �궨��	������Χ0~65535

//****************************************IO�˿ڶ���***************************************
sbit CE   = P2^5;
sbit SCK  = P2^4;
sbit MISO = P2^3;
sbit CSN  = P2^0;
sbit MOSI = P2^1;
sbit IRQ  = P2^2;



//******************************************************************************************
uchar  bdata sta;   //״̬��־
sbit RX_DR =sta^6;
sbit TX_DS =sta^5;
sbit MAX_RT =sta^4;
//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5    // 5 uints TX address width
#define RX_ADR_WIDTH    5    // 5 uints RX address width
#define TX_PLOAD_WIDTH  32   // 32 uints TX payload
#define RX_PLOAD_WIDTH  32   // 32 uints TX payload
uchar code TX_ADDRESS[TX_ADR_WIDTH]= {0x68,0x86,0x66,0x88,0x28}; 
uchar code RX_ADDRESS[RX_ADR_WIDTH]= {0x68,0x86,0x66,0x88,0x28}; 
uchar Tx_Buf[TX_PLOAD_WIDTH]={0x01,0x02,0x03,0x22,0x33,0xaa,0xbb,0x11,0x22,0x33,0xaa,0xbb,0x11,0x22,
0x33,0xaa,0xbb,0x11,0x22,0x33,0xaa,0xbb,0x11,0x22,0x33,0xaa,0xbb,0x11,0x22,0x33,0xee,0xff};//��������
uchar Rx_Buf[RX_PLOAD_WIDTH];//��������
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define READ_REG        0x00   // ���Ĵ���ָ��
#define WRITE_REG       0x20  // д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61   // ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0   // д��������ָ��
#define FLUSH_TX        0xE1  // ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2   // ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3   // �����ظ�װ������ָ��
#define NOP             0xFF   // ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
/******************************************��ʱ����********************************************************/
//����ʱ
void Delay(unsigned int s)
{
	unsigned int i,j;
	for(i=0;i<1000;i++)
		for(j=0;j<s;j++);
}
//����ʱ
void delay_ms(unsigned int x)
{
	unsigned int i,j;
	for(i=0;i<x;i++)
	{
		j=108;;
		while(j--);
	}
}
/************************************IO ��ģ��SPI���� ����************************************************/
uchar SPI_RW(uchar byte)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		MOSI=(byte&0x80);		
		byte=(byte<<1);
		SCK=1;
		byte|=MISO;
		//led=MISO;Delay(150);
		SCK=0;
	}
	return(byte);
}
uchar SPI_RW_Reg(uchar reg,uchar value) // ��Ĵ���REGдһ���ֽڣ�ͬʱ����״̬�ֽ�
{
	uchar status;
	CSN=0;
	status=SPI_RW(reg);
	SPI_RW(value);
	CSN=1;
	return(status);
}
uchar SPI_Read (uchar  reg )
{
	uchar reg_val;
	CSN=0;
	SPI_RW(reg);
	reg_val=SPI_RW(0);
	CSN=1;
	return(reg_val);
}

uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar bytes)
{
	uchar status,byte_ctr;
	CSN = 0;                   // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);    // ѡ��Ĵ���д��Ͷ�ȡ״̬�ֽ�
	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // Ȼ��д�����ֽ��ڻ�����(* pBuf)
	SPI_RW(*pBuf++);
	CSN = 1;                 // Set CSN high again
	return(status);          // nRF24L01����״̬�ֽ�
}

/*******************************��*****��*****ģ*****ʽ*****��*****��*************************************/
void TX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(FLUSH_TX,0x00);
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // tx��ַ��nRF24L01д��
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 tx adrһ��Ϊ�Զ�ack
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 �ط�...1a
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // ѡ��RF�ŵ�40 
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ2�ֽ�
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);
	CE=1;
	delay_ms(100);
}

void Transmit(unsigned char * tx_buf)  
{	 //����
	CE=0;   //StandBy Iģʽ 
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_RW_Reg(FLUSH_TX,0x00);
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // װ������ 
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // IRQ�շ�����ж���Ӧ��16λCRC��������
	CE=1;   //�ø�CE���������ݷ���
	delay_ms(150);
}
/*******************************��*****��*****ģ*****ʽ*****��*****��*************************************/
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uchar status,i;								   // ����
	CSN = 0;                      // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);         // ѡ��Ĵ���д��Ͷ�ȡ��״̬ uchar
	for(i=0;i<uchars;i++)
		pBuf[i] = SPI_RW(0);    // 
	CSN = 1;       
	return(status);                    // return nRF24L01 status uchar
}
/******************************************************************************************************/
/*������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
	unsigned char revale=0;
	sta=SPI_Read(STATUS); // ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(RX_DR)             // �ж��Ƿ���յ�����
	{
		CE = 0;    //SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;   //��ȡ������ɱ�־
		//Delay(100);
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	return revale;
}
/****************************************************************************************************/
/*������void RX_Mode(void)
/*���ܣ����ݽ������� 
/****************************************************************************************************/
void RX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(FLUSH_RX,0x00);
	//SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack 
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
	//SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...1a
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0F);
	CE=1;
	delay_ms(130);
}
//************************************���ڳ�ʼ��*********************************************************
void StartUART( void )
{         //������9600
	SCON = 0x50;
	TMOD = 0x20;
	TH1 = 0xFD;
	TL1 = 0xFD;
	PCON = 0x00;
	TR1 = 1;
}
//************************************ͨ�����ڽ����յ����ݷ��͸�PC��**************************************
void R_S_Byte(uchar R_Byte)
{ 
	SBUF = R_Byte;  
	while( TI == 0 );    //��ѯ��
	TI = 0;    
}
#endif