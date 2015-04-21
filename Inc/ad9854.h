#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define AD9854_RST0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0001, GPIO_PIN_RESET)
#define AD9854_RST1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0001, GPIO_PIN_SET)
#define AD9854_UDCLK0  HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0002, GPIO_PIN_RESET)//AD9854����ʱ��
#define AD9854_UDCLK1  HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0002, GPIO_PIN_SET)//AD9854����ʱ��
#define AD9854_WR0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0004, GPIO_PIN_RESET)
#define AD9854_WR1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0004, GPIO_PIN_SET)
#define AD9854_RD0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0008, GPIO_PIN_RESET)
#define AD9854_RD1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0008, GPIO_PIN_SET)
#define AD9854_FDATA0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0010, GPIO_PIN_RESET)
#define AD9854_FDATA1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0010, GPIO_PIN_SET)
#define AD9854_OSK0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0020, GPIO_PIN_RESET)
#define AD9854_OSK1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0020, GPIO_PIN_SET)
#define uint  unsigned int
#define uchar unsigned char
#define ulong unsigned long
#define      CLK_Set            6
const ulong  Freq_mult_ulong  = 2345625;
const double Freq_mult_doulle = 2345624.80592213;
//sbit AD9854_WR     = P3^5;   //AD9854дʹ�ܣ�����Ч
//sbit AD9854_RD     = P3^4;   //AD9854��ʹ�ܣ�����Ч
//sbit AD9854_FDATA  = P3^3;   //AD9854 FSK,PSK����
//sbit AD9854_OSK    = P3^2;   //AD9854 OSK���ƶ�
unsigned char FreqWord[6];
void delay (uint us);
//====================================================================================
//��������:void AD9854_WR_Byte(unsigned char addr,unsigned char dat)
//��������:AD9854���п�д������
//��ڲ���:addr     6λ��ַ
//         dat      д�������
//���ڲ���:��
//====================================================================================
void AD9854_WR_Byte(unsigned char addr,unsigned char dat)
{
	HAL_GPIO_WritePin(GPIOC,0x003f,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,0x00ff,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,0x003f|addr,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,0x00ff|dat,GPIO_PIN_SET);
	AD9854_WR0;
	delay(10);
	AD9854_WR1;	
}

//====================================================================================
//��������:void AD9854_Init(void)
//��������:AD9854��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_Init(void)
{
	AD9854_WR1;//������д���ƶ˿���Ϊ��Ч
    AD9854_RD1;
    AD9854_UDCLK0;
    AD9854_RST1;                 //��λAD9854
    AD9854_RST0;

	AD9854_WR_Byte(0x1d,0x10);	           //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	   //����ϵͳʱ�ӱ�Ƶ            
	AD9854_WR_Byte(0x1f,0x00);	   //����ϵͳΪģʽ0�����ⲿ����
	AD9854_WR_Byte(0x20,0x60);	   //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	AD9854_UDCLK1;               //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void Freq_convert(long Freq)
//��������:�����ź�Ƶ������ת��
//��ڲ���:Freq   ��Ҫת����Ƶ�ʣ�ȡֵ��0~SYSCLK/2
//���ڲ���:��   ����Ӱ��ȫ�ֱ���FreqWord[6]��ֵ
//˵����   ���㷨λ���ֽ�����㷨���й�ʽFTW = (Desired Output Frequency �� 2N)/SYSCLK
//         �õ����㷨������N=48��Desired Output Frequency Ϊ����Ҫ��Ƶ�ʣ���Freq��SYSCLK
//         Ϊ�ɱ�̵�ϵͳʱ�ӣ�FTWΪ48Bit��Ƶ�ʿ����֣���FreqWord[6]
//====================================================================================
void Freq_convert(long Freq)   
{
    ulong FreqBuf;
    ulong Temp=Freq_mult_ulong;   	       

	unsigned char Array_Freq[4];			     //������Ƶ�����ӷ�Ϊ�ĸ��ֽ�
	Array_Freq[0]=(unsigned char)Freq;
	Array_Freq[1]=(unsigned char)(Freq>>8);
	Array_Freq[2]=(unsigned char)(Freq>>16);
	Array_Freq[3]=(unsigned char)(Freq>>24);

	FreqBuf=Temp*Array_Freq[0];                  
    FreqWord[0]=FreqBuf;    
    FreqBuf>>=8;

    FreqBuf+=(Temp*Array_Freq[1]);
    FreqWord[1]=FreqBuf;
    FreqBuf>>=8;

    FreqBuf+=(Temp*Array_Freq[2]);
    FreqWord[2]=FreqBuf;
    FreqBuf>>=8;

	FreqBuf+=(Temp*Array_Freq[3]);
    FreqWord[3]=FreqBuf;
    FreqBuf>>=8;

    FreqWord[4]=FreqBuf;
    FreqWord[5]=FreqBuf>>8;			
}  

//====================================================================================
//��������:void AD9854_SetSine(ulong Freq,uint Shape)
//��������:AD9854���Ҳ���������
//��ڲ���:Freq   Ƶ�����ã�ȡֵ��ΧΪ0~(1/2)*SYSCLK
//         Shape  ��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095) ,ȡֵԽ��,����Խ�� 
//���ڲ���:��
//====================================================================================
void AD9854_SetSine(ulong Freq,uint Shape)
{
	unsigned char count;
	unsigned char Adress;

	Adress = 0x04;                        //ѡ��Ƶ�ʿ����ֵ�ַ�ĳ�ֵ

	Freq_convert(Freq);		           //Ƶ��ת��

	for(count=6;count>0;)	          //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }
	
	AD9854_WR_Byte(0x21,Shape>>8);	  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	  //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                    //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void Freq_doublt_convert(double Freq)
//��������:�����ź�Ƶ������ת��
//��ڲ���:Freq   ��Ҫת����Ƶ�ʣ�ȡֵ��0~SYSCLK/2
//���ڲ���:��   ����Ӱ��ȫ�ֱ���FreqWord[6]��ֵ
//˵����   �й�ʽFTW = (Desired Output Frequency �� 2N)/SYSCLK�õ��ú�����
//         ����N=48��Desired Output Frequency Ϊ����Ҫ��Ƶ�ʣ���Freq��SYSCLK
//         Ϊ�ɱ�̵�ϵͳʱ�ӣ�FTWΪ48Bit��Ƶ�ʿ����֣���FreqWord[6]
//ע�⣺   �ú��������溯��������Ϊ�ú�������ڲ���Ϊdouble����ʹ�źŵ�Ƶ�ʸ���ȷ
//         ���꽨����100HZ�����ñ��������ڸ���100HZ��������ú���void Freq_convert(long Freq)
//====================================================================================
void Freq_double_convert(double Freq)   
{
	ulong Low32;
	uint  High16;
    double Temp=Freq_mult_doulle;   	            //23ca99Ϊ2��48�η�����120M
	Freq*=(double)(Temp);
//	1 0000 0000 0000 0000 0000 0000 0000 0000 = 4294967295
	High16 = (int)(Freq/4294967295);                  //2^32 = 4294967295
	Freq -= (double)High16*4294967295;
	Low32 = (ulong)Freq;

    FreqWord[0]=Low32;	     
    FreqWord[1]=Low32>>8;
    FreqWord[2]=Low32>>16;
    FreqWord[3]=Low32>>24;
    FreqWord[4]=High16;
    FreqWord[5]=High16>>8;			
} 

//====================================================================================
//��������:void AD9854_SetSine_double(double Freq,uint Shape)
//��������:AD9854���Ҳ���������
//��ڲ���:Freq   Ƶ�����ã�ȡֵ��ΧΪ0~1/2*SYSCLK
//         Shape  ��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095) 
//���ڲ���:��
//====================================================================================
void AD9854_SetSine_double(double Freq,uint Shape)
{
	unsigned char count=0;
	unsigned char Adress;

	Adress=0x04;						     //ѡ��Ƶ�ʿ�����1��ַ�ĳ�ֵ

	Freq_double_convert(Freq);		           //Ƶ��ת��
	 
	for(count=6;count>0;)	                    //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }
	
	AD9854_WR_Byte(0x21,Shape>>8);	  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	  //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                    //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_InitFSK(void)
//��������:AD9854��FSK��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitFSK(void)
{
	AD9854_WR1;                        //������д���ƶ˿���Ϊ��Ч
    AD9854_RD1;
    AD9854_UDCLK0;
    AD9854_RST1;                        //��λAD9854
    AD9854_RST0;	

	AD9854_WR_Byte(0x1d,0x10);	       //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	   //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x02);	       //����ϵͳΪģʽ1�����ⲿ����
	AD9854_WR_Byte(0x20,0x60);	      //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	AD9854_UDCLK1;                  //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_SetFSK(ulong Freq1,ulong Freq2)
//��������:AD9854��FSK����
//��ڲ���:Freq1   FSKƵ��1   
//         Freq2   FSKƵ��2
//���ڲ���:��
//====================================================================================
void AD9854_SetFSK(ulong Freq1,ulong Freq2)
{
    unsigned char count=6;
	unsigned char Adress1,Adress2;

	const uint Shape=4000;	      //��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095)
	
	Adress1=0x04;				 //ѡ��Ƶ�ʿ�����1��ַ�ĳ�ֵ
	Adress2=0x0a;				 //ѡ��Ƶ�ʿ�����2��ַ�ĳ�ֵ
	
	Freq_convert(Freq1);               //Ƶ��ת��1
	
	for(count=6;count>0;)	          //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress1++,FreqWord[--count]);
    }
	
	Freq_convert(Freq2);               //Ƶ��ת��2

	for(count=6;count>0;)	          //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress2++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x21,Shape>>8);	      //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	     //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                    //����AD9854���
    AD9854_UDCLK0;		
}

//====================================================================================
//��������:void AD9854_InitBPSK(void)
//��������:AD9854��BPSK��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitBPSK(void)
{
	AD9854_WR1;                    //������д���ƶ˿���Ϊ��Ч
    AD9854_RD1;
    AD9854_UDCLK0;
    AD9854_RST1;                   //��λAD9854
    AD9854_RST0;

	AD9854_WR_Byte(0x1d,0x10);	       //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	   //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x08);	      //����ϵͳΪģʽ4�����ⲿ����
	AD9854_WR_Byte(0x20,0x60);	      //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	AD9854_UDCLK1;                //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_SetBPSK(uint Phase1,uint Phase2)
//��������:AD9854��BPSK����
//��ڲ���:Phase1   ������λ1
//         Phase2	������λ2
//���ڲ���:��
//˵����   ��λΪ14Bit��ȡֵ��0~16383�����꽨�����ñ�������ʱ��Phase1����Ϊ0��
//         ��Phase1����Ϊ8192��180����λ
//====================================================================================
void AD9854_SetBPSK(uint Phase1,uint Phase2)
{
	unsigned char count;

	const ulong Freq=60000;
    const uint Shape=4000;

	unsigned char Adress;
	Adress=0x04;                           //ѡ��Ƶ�ʿ�����1��ַ�ĳ�ֵ

	AD9854_WR_Byte(0x00,Phase1>>8);	           //������λ1
	AD9854_WR_Byte(0x01,(unsigned char)(Phase1&0xff));
	
	AD9854_WR_Byte(0x02,Phase2>>8);	          //������λ2
	AD9854_WR_Byte(0x03,(unsigned char)(Phase2&0xff));

	Freq_convert(Freq);                            //Ƶ��ת��

	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x21,Shape>>8);	                  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	               //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                                //����AD9854���
		delay(5);
    AD9854_UDCLK0;	
}

//====================================================================================
//��������:void AD9854_InitOSK(void)
//��������:AD9854��OSK��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitOSK(void)
{
	AD9854_WR1;                           //������д���ƶ˿���Ϊ��Ч
	AD9854_RD1;
	AD9854_UDCLK0;
	AD9854_RST1;                          //��λAD9854
	AD9854_RST0;

    AD9854_WR_Byte(0x1d,0x10);	           //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	       //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x00);	           //����ϵͳΪģʽ0�����ⲿ����
	AD9854_WR_Byte(0x20,0x70);	           //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����,ͨ�������ڲ�����

	AD9854_UDCLK1;                        //����AD9854���
	AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_SetOSK(unsigned char RateShape)
//��������:AD9854��OSK����
//��ڲ���: RateShape    OSKб��,ȡֵΪ4~255��С��4����Ч
//���ڲ���:��
//====================================================================================
void AD9854_SetOSK(unsigned char RateShape)
{
	unsigned char count;

	const ulong Freq=60000;			 //������Ƶ
    const uint  Shape=4000;			//��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095)

	unsigned char Adress;
	Adress=0x04;               //ѡ��Ƶ�ʿ����ֵ�ַ�ĳ�ֵ

	Freq_convert(Freq);                       //Ƶ��ת��

	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x21,Shape>>8);	                  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));  	 


    AD9854_WR_Byte(0x25,RateShape);				       //����OSKб��

	AD9854_UDCLK1;                                //����AD9854���
    AD9854_UDCLK0;	
}

//====================================================================================
//��������:void AD9854_InitAM(void)
//��������:AD9854��AM��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitAM(void)
{
	unsigned char count;

	const ulong Freq=60000;			 //������Ƶ

	unsigned char  Adress;
	Adress=0x04;      //ѡ��Ƶ�ʿ����ֵ�ַ�ĳ�ֵ
	
    AD9854_WR1;    //������д���ƶ˿���Ϊ��Ч
    AD9854_RD1;
    AD9854_UDCLK0;
    AD9854_RST1;     //��λAD9854
    AD9854_RST0;

	AD9854_WR_Byte(0x1d,0x10);	                  //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	             //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x00);	                 //����ϵͳΪģʽ0�����ⲿ����
	AD9854_WR_Byte(0x20,0x60);	                  //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	Freq_convert(Freq);                            //Ƶ��ת��

	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }

	AD9854_UDCLK1;                             //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_SetAM(unsigned char Shape)
//��������:AD9854��AM����
//��ڲ���:Shape   12Bit����,ȡֵ��0~4095   
//���ڲ���:��
//====================================================================================
void AD9854_SetAM(uint Shape)
{
	AD9854_WR_Byte(0x21,Shape>>8);	                  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                                   //����AD9854���
    AD9854_UDCLK0;			
}

//====================================================================================
//��������:void AD9854_InitRFSK(void)
//��������:AD9854��RFSK��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitRFSK(void)
{
	AD9854_WR1;                      //������д���ƶ˿���Ϊ��Ч
    AD9854_RD1;
    AD9854_UDCLK0;
    AD9854_RST1;                     //��λAD9854
    AD9854_RST0;
	
	AD9854_WR_Byte(0x1d,0x10);	       //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	   //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x24);	        //����ϵͳΪģʽ2�����ⲿ����,ʹ�����ǲ�ɨƵ����
	AD9854_WR_Byte(0x20,0x60);	        //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����	

	AD9854_UDCLK1;                   //����AD9854���
    AD9854_UDCLK0;
}

//====================================================================================
//��������:void AD9854_SetRFSK(void)
//��������:AD9854��RFSK����
//��ڲ���:Freq_Low          RFSK��Ƶ��	   48Bit
//         Freq_High         RFSK��Ƶ��	   48Bit
//         Freq_Up_Down		 ����Ƶ��	   48Bit
//		   FreRate           б��ʱ�ӿ���  20Bit
//���ڲ���:��
//ע��     ÿ��������֮���ʱ����������ʽ��ʾ��FreRate +1��*��System Clock ����һ������,
//         Ƶ�� ���������½� һ������Ƶ��
//====================================================================================
void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate)
{
	unsigned char count=6;
	unsigned char Adress1,Adress2,Adress3;
    const uint  Shape=4000;			   //��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095)

	Adress1=0x04;		     //ѡ��Ƶ�ʿ����ֵ�ַ�ĳ�ֵ 
	Adress2=0x0a;
	Adress3=0x10;

	Freq_convert(Freq_Low);                             //Ƶ��1ת��

	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress1++,FreqWord[--count]);
    }

	Freq_convert(Freq_High);                             //Ƶ��2ת��

	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress2++,FreqWord[--count]);
    }

	Freq_convert(Freq_Up_Down);                             //����Ƶ��ת��

	for(count=6;count>0;)	                               //д��6�ֽڵ�Ƶ�ʿ�����  
    {
		AD9854_WR_Byte(Adress3++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x1a,(unsigned char)((FreRate>>16)&0x0f));				   //����б������
	AD9854_WR_Byte(0x1b,(unsigned char)(FreRate>>8));
	AD9854_WR_Byte(0x1c,(unsigned char)FreRate);				    

	AD9854_WR_Byte(0x21,Shape>>8);	                  //����Iͨ������
	AD9854_WR_Byte(0x22,(unsigned char)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //����Qͨ������
	AD9854_WR_Byte(0x24,(unsigned char)(Shape&0xff));

	AD9854_UDCLK1;                                //����AD9854���
  AD9854_UDCLK0;	
}

//================================================================= 
// �������� ��void delay (uint us)
// �������� ��us����ʱ,���2~4 us
// ��ڲ��� ��us  ��ʱʱ��ĳ���
// ���ڲ��� ����
//================================================================= 
void delay (uint us)   
{
	uint i,j;
	for(i=0;i<us;i++)
		for(j=0;j<12;j++);
}

//�������Ҳ�������120MHZ SYSCLKʱ,����10MHZ���Σ����κܺã����Գɹ�
//������300MHZ SYSCLKʱ,����50MHZ����ʱ,DDS��������,�Ҳ���˥������,������35mV����

//int main()
//{
//	AD9854_Init();
//	AD9854_SetSine(10000000,4000);
//	while(1);			
//}


//�������Ҳ�������120MHZ SYSCLKʱ,����87.697HZ���Σ����κܺã����Գɹ�
/*
int main()
{
	AD9854_Init();
	AD9854_SetSine_double(87.697,4000);
	while(1);			
} 
*/

//����FSK������120MHZ SYSCLK,1K��6K,���Գɹ�,�����Ӧ"FSK����.bmp"
/*
int main()
{
	AD9854_InitFSK();
	AD9854_SetFSK(1000,6000);
	while(1)
	{
		AD9854_FDATA = 1;
		delay(30000);	      //��ʱʱ�䳤�����ڹ۲�
		AD9854_FDATA = 0;
		delay(30000);
	}			
} 
*/

//����BPSK������120MHZ SYSCLK,���Գɹ�
/*
int main()
{
	AD9854_InitBPSK();
	AD9854_SetBPSK(0,8192);
	while(1)
	{
		AD9854_FDATA = 1;
		delay(10);	      
		AD9854_FDATA = 0;
		delay(10);
	}			
} 
*/


//����OSK������120MHZ SYSCLK,���Գɹ�
/*
int main()
{
	AD9854_InitOSK();
	AD9854_SetOSK(10);
	while(1)
	{
		AD9854_OSK=1;	
		delay(30); 
		AD9854_OSK=0;	
		delay(30);
	}			
} 
*/

//����AM������120MHZ SYSCLK,���Գɹ�
/*
int main()
{
	AD9854_InitAM();
	while(1)
	{
		AD9854_SetAM(2000);
		delay(10);	      
		AD9854_SetAM(4000);
		delay(10);
	}			
}
*/

//����RFSK������120MHZ SYSCLK,���Գɹ�
/*
int main()
{
	AD9854_InitRFSK();
	AD9854_SetRFSK(1000,60000,100,30);
	while(1)
	{
		AD9854_FDATA = 1;
		delay(30000);	      //��ʱʱ�䳤�����ڹ۲�
		AD9854_FDATA = 0;
		delay(30000);
	} 
}			
*/
