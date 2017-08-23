#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define AD9854_RST0 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0001, GPIO_PIN_RESET)
#define AD9854_RST1 HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0001, GPIO_PIN_SET)
#define AD9854_UDCLK0  HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0002, GPIO_PIN_RESET)//AD9854更新时钟
#define AD9854_UDCLK1  HAL_GPIO_WritePin(GPIOE,(uint16_t)0x0002, GPIO_PIN_SET)//AD9854更新时钟
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

extern const ulong  Freq_mult_ulong;
extern const double Freq_mult_doulle;
//====================================================================================
//函数名称:void AD9854_WR_Byte(unsigned char addr,unsigned char dat)
//函数功能:AD9854并行口写入数据
//入口参数:addr     6位地址
//         dat      写入的数据
//出口参数:无
//====================================================================================
void AD9854_WR_Byte(unsigned char addr,unsigned char dat);

//====================================================================================
//函数名称:void AD9854_Init(void)
//函数功能:AD9854初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_Init(void);

//====================================================================================
//函数名称:void Freq_convert(long Freq)
//函数功能:正弦信号频率数据转换
//入口参数:Freq   需要转换的频率，取值从0~SYSCLK/2
//出口参数:无   但是影响全局变量FreqWord[6]的值
//说明：   该算法位多字节相乘算法，有公式FTW = (Desired Output Frequency × 2N)/SYSCLK
//         得到该算法，其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
//         为可编程的系统时钟，FTW为48Bit的频率控制字，即FreqWord[6]
//====================================================================================
void Freq_convert(long Freq);   

//====================================================================================
//函数名称:void AD9854_SetSine(ulong Freq,uint Shape)
//函数功能:AD9854正弦波产生程序
//入口参数:Freq   频率设置，取值范围为0~(1/2)*SYSCLK
//         Shape  幅度设置. 为12 Bit,取值范围为(0~4095) ,取值越大,幅度越大 
//出口参数:无
//====================================================================================
void AD9854_SetSine(ulong Freq,uint Shape);

//====================================================================================
//函数名称:void Freq_doublt_convert(double Freq)
//函数功能:正弦信号频率数据转换
//入口参数:Freq   需要转换的频率，取值从0~SYSCLK/2
//出口参数:无   但是影响全局变量FreqWord[6]的值
//说明：   有公式FTW = (Desired Output Frequency × 2N)/SYSCLK得到该函数，
//         其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
//         为可编程的系统时钟，FTW为48Bit的频率控制字，即FreqWord[6]
//注意：   该函数与上面函数的区别为该函数的入口参数为double，可使信号的频率更精确
//         谷雨建议在100HZ以下用本函数，在高于100HZ的情况下用函数void Freq_convert(long Freq)
//====================================================================================
void Freq_double_convert(double Freq);   

//====================================================================================
//函数名称:void AD9854_SetSine_double(double Freq,uint Shape)
//函数功能:AD9854正弦波产生程序
//入口参数:Freq   频率设置，取值范围为0~1/2*SYSCLK
//         Shape  幅度设置. 为12 Bit,取值范围为(0~4095) 
//出口参数:无
//====================================================================================
void AD9854_SetSine_double(double Freq,uint Shape);

//====================================================================================
//函数名称:void AD9854_InitFSK(void)
//函数功能:AD9854的FSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitFSK(void);

//====================================================================================
//函数名称:void AD9854_SetFSK(ulong Freq1,ulong Freq2)
//函数功能:AD9854的FSK设置
//入口参数:Freq1   FSK频率1   
//         Freq2   FSK频率2
//出口参数:无
//====================================================================================
void AD9854_SetFSK(ulong Freq1,ulong Freq2);

//====================================================================================
//函数名称:void AD9854_InitBPSK(void)
//函数功能:AD9854的BPSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitBPSK(void);

//====================================================================================
//函数名称:void AD9854_SetBPSK(uint Phase1,uint Phase2)
//函数功能:AD9854的BPSK设置
//入口参数:Phase1   调制相位1
//         Phase2	调制相位2
//出口参数:无
//说明：   相位为14Bit，取值从0~16383，谷雨建议在用本函数的时候将Phase1设置为0，
//         将Phase1设置为8192，180°相位
//====================================================================================
void AD9854_SetBPSK(uint Phase1,uint Phase2);

//====================================================================================
//函数名称:void AD9854_InitOSK(void)
//函数功能:AD9854的OSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitOSK(void);

//====================================================================================
//函数名称:void AD9854_SetOSK(unsigned char RateShape)
//函数功能:AD9854的OSK设置
//入口参数: RateShape    OSK斜率,取值为4~255，小于4则无效
//出口参数:无
//====================================================================================
void AD9854_SetOSK(unsigned char RateShape);

//====================================================================================
//函数名称:void AD9854_InitAM(void)
//函数功能:AD9854的AM初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitAM(void);

//====================================================================================
//函数名称:void AD9854_SetAM(unsigned char Shape)
//函数功能:AD9854的AM设置
//入口参数:Shape   12Bit幅度,取值从0~4095   
//出口参数:无
//====================================================================================
void AD9854_SetAM(uint Shape);

//====================================================================================
//函数名称:void AD9854_InitRFSK(void)
//函数功能:AD9854的RFSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitRFSK(void);

//====================================================================================
//函数名称:void AD9854_SetRFSK(void)
//函数功能:AD9854的RFSK设置
//入口参数:Freq_Low          RFSK低频率	   48Bit
//         Freq_High         RFSK高频率	   48Bit
//         Freq_Up_Down		 步进频率	   48Bit
//		   FreRate           斜率时钟控制  20Bit
//出口参数:无
//注：     每两个脉冲之间的时间周期用下式表示（FreRate +1）*（System Clock ），一个脉冲,
//         频率 上升或者下降 一个步进频率
//====================================================================================
void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate);

//================================================================= 
// 函数名称 ：void delay (uint us)
// 函数功能 ：us级延时,大概2~4 us
// 入口参数 ：us  延时时间的长短
// 出口参数 ：无
//================================================================= 
void delay (uint us);

//测试正弦波，采用120MHZ SYSCLK时,出来10MHZ波形，波形很好，测试成功
//当采用300MHZ SYSCLK时,测试50MHZ波形时,DDS发热厉害,且波形衰减严重,幅度在35mV左右

//int main()
//{
//	AD9854_Init();
//	AD9854_SetSine(10000000,4000);
//	while(1);			
//}


//测试正弦波，采用120MHZ SYSCLK时,出来87.697HZ波形，波形很好，测试成功
/*
int main()
{
	AD9854_Init();
	AD9854_SetSine_double(87.697,4000);
	while(1);			
} 
*/

//测试FSK，采用120MHZ SYSCLK,1K和6K,测试成功,结果对应"FSK波形.bmp"
/*
int main()
{
	AD9854_InitFSK();
	AD9854_SetFSK(1000,6000);
	while(1)
	{
		AD9854_FDATA1;
		delay(30000);	      //延时时间长，便于观察
		AD9854_FDATA0;
		delay(30000);
	}			
} 
*/

//测试BPSK，采用120MHZ SYSCLK,测试成功
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


//测试OSK，采用120MHZ SYSCLK,测试成功
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

//测试AM，采用120MHZ SYSCLK,测试成功
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

//测试RFSK，采用120MHZ SYSCLK,测试成功
/*
int main()
{
	AD9854_InitRFSK();
	AD9854_SetRFSK(1000,60000,100,30);
	while(1)
	{
		AD9854_FDATA = 1;
		delay(30000);	      //延时时间长，便于观察
		AD9854_FDATA = 0;
		delay(30000);
	} 
}			
*/
