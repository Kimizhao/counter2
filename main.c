/*
 *
 */
#include "common.h"
#include "at93c66.h"

unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//01234567
unsigned char code arabic[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};/* "0","1","2","3","4","5","6","7","8","9" */
unsigned char code special[]={0x40,0x08,0x79,0x77,0x7c,0x39,0x5e,0x71,0x3d,0x76};/* "-","_","E","A","b","C","d","F","G","H" */
unsigned char code seg[]={0,1,2,3,4,5,6,7};//

unsigned char DisThous;
unsigned char DisHunder;
unsigned char DisTens;
unsigned char DisSingle;
unsigned int curnum;
unsigned int num;

//sbit CNUP=P1^0;
// sbit CNDOWN=P1^1;
// sbit CNNUN=P1^3;
//bit newa,newb;

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
	bit cna,cnb;
	curnum=0;
	
	num=1234;

	DisSingle=table[num%10];
	DisTens=table[(num/10)%10];
	DisHunder=table[(num/100)%10];
	DisThous=table[(num/1000)%10];
	
	P0=0x3f;
	delay(60000);

	//P0=EEPROM_Read(0x00);
	P2=0;
	
/* 	SerialDataOutput(0x06);// "1"
	SerialDataOutput(0x5b);// "2"
	SerialDataOutput(0x4f);// "3"
	SerialDataOutput(0x66);// "4"
	SerialDataOutput(0x06);// "1" */
	
/* 	for (i=0;i<2;i++)
	{
		SerialDataOutput(special[i+8]);
	} */
	
	while (1)
	{ }
}

void Timer0_isr(void) interrupt 1 using 1
{
	TH0=0x00;		/* Init value */
	TL0=0x00;
}
