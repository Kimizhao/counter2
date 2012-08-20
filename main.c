/*
 *
 */
#include "common.h"

unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//01234567
unsigned char code seg[]={0,1,2,3,4,5,6,7};//

unsigned char DisThous;
unsigned char DisHunder;
unsigned char DisTens;
unsigned char DisSingle;
unsigned int curnum;
unsigned int num;


void delay(unsigned int cnt)
{
	while(--cnt);
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
	//unsigned int num;
	/* 定时/计数器2 作为外部脉冲计数初始化 */
	//Init_counter2();
	
	curnum=0;

	num=1234;
	
	DisSingle=table[num%10];
	DisTens=table[(num/10)%10];
	DisHunder=table[(num/100)%10];
	DisThous=table[(num/1000)%10];
	
	while (1)
	{
		P0=DisThous;
		P2=0;
		delay(300);
		P0=DisHunder;
		P2=1;
		delay(300);
		P0=DisTens;
		P2=2;
		delay(300);
		P0=DisSingle;
		P2=3;
		delay(300);
		
		
		//
		num=TH2*256+TL2;
		if (curnum!=num)
		{
			DisSingle=table[num%10];
			DisTens=table[(num/10)%10];
			DisHunder=table[(num/100)%10];
			DisThous=table[(num/1000)%10];
			curnum=num;
		}

	}
	
}