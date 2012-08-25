/*
 *
 */
#include "common.h"
#include "at93c66.h"
#include "scaner.h"

unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//01234567
unsigned char code seg[]={0,1,2,3,4,5,6,7};//

unsigned char DisThous;
unsigned char DisHunder;
unsigned char DisTens;
unsigned char DisSingle;
unsigned int curnum;
unsigned char j;
unsigned int num;
unsigned long num2;

sbit KEY_UP=P1^6;
sbit KEY_DOWN=P1^5;
sbit led=P1^7;
bit isLowPower;
bit isFlash;

extern void MBI_Shift_32_bit(unsigned long datal);

void delay(unsigned int cnt)
{
	while(--cnt);
}

void DelayUs2x(unsigned char t)
{   
 while(--t);
}

void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     DelayUs2x(245);
     DelayUs2x(245);
 }
}

void Init_Timer0(void)
{
	TMOD |= 0x01;			     
	TH0=0x00;	              	/* Init value */
	TL0=0x00;
	EA=1;                      /* interupt enable */
	ET0=1;                     /* enable timer0 interrupt */
	TR0=1;
}

void Init_T0(void)
{
	//P1=0x55;//P1口初始值
	EA=1;//全局中断开
	EX0=1;//外部中断0开
	IT0=1;//边沿触发
}

void Init_counter2(void)
{
	RCAP2H = (65536-60000)/256;//@12M 10ms 16bit 自动重载
	RCAP2L = (65536-60000)%256;
	
	//T2MOD
	//T2MOD|=0x00;???
	T2MOD|=0x01;//定时器2输入使能位：0 向下计数使能位：1
	
	//T2CON
	C_T2=1; //外部脉冲计数
	EXEN2=1; //关闭T2Ex跳变有效位
	TR2=1; //counter2开始工作
	
}



void main(void)
{
	unsigned char i;
	unsigned char key;
	num=1234;
	//Init_Timer0();
	Init_T0();
	
	
	AT93CXX_SPI_PORT_INIT();
 	for (i=0;i<8;i++)
	{
		delay(60000);
		P0=AT93CXX_Read_Data(i);
		P2=i;
	}
	
	MBI_Shift_32_bit(1234);
	num=AT93CXX_Read_Data(0)+AT93CXX_Read_Data(1)*256;
	P0=table[2];
	P2=1;
	delay(60000);
	P0=table[8];
	P2=1;
	delay(60000);
	
	while (1)
	{
		key=keyscan();
		switch (key)
		{
			delay(200);
			case 0x7e:
				MBI_Shift_32_bit(234);
			break;
			case 0x7d:
				MBI_Shift_32_bit(674);
			break;
			case 0xdd:
				num++;
			break;
			case 0xdb:
				num--;
			break;
		default:
			break;
		}
		
		//MBI_Shift_32_bit(num);
		
		if (isLowPower)
		{
			isLowPower=0;
			AT93CXX_SPI_PORT_INIT();
			AT93CXX_EN_Write();
			AT93CXX_Write_Data(0,num&0x00ff);
			AT93CXX_Write_Data(1,num>>8);

		}
		
 		if(!KEY_UP)  
		{
			DelayMs(10);
			if(!KEY_UP)     
			{
				while (!KEY_UP);
				{
					num++;				
				}
			}
		} 
		
 		if(!KEY_DOWN)  
		{
			DelayMs(10);
			if(!KEY_DOWN)     
			{
				while (!KEY_DOWN);
				{
					num--;
				}
			}
		}
		
		if (isFlash)
		{
			MBI_Shift_32_bit(10000);
		}else{
			MBI_Shift_32_bit(num);
		}
	}
}

void ISR_Timer0(void) interrupt 1 using 1
{
	TH0=0x00;		/* Init value */
	TL0=0x00;
}

void ISR_Timer2(void) interrupt 5 using 1
{
	static unsigned char tcn=0;
	TF2=0;
	tcn++;
	if (tcn==50)
	{
		tcn=0;
		isFlash=~isFlash;
	}
	
}

void ISR_LowPower(void) interrupt 0 using 1
{
	isLowPower=1;
	led=~led;
}
