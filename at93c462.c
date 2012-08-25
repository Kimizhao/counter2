#include "common.h"

/*
 * 3WI: Write Operation:
 * Enable device via chip select high
 * Send write enable op code
 * Disable device via chip select low
 * Enable device via chip select high
 * Send write op code with address
 * Send data byte/word
 * Write cycle begins
 * Disable device via chip select low
 *
 * Read Operation:
 * Enable device via chip select high
 * Send read op code with address
 * Receive data byte(s)
 * Disable device via chip select low
 */

#define EEPROM_EWEN 0x98

sbit EEPROM_CS=P1^0;
sbit EEPROM_CLK=P1^1;
sbit EEPROM_DI=P1^2;
sbit EEPROM_DO=P1^3;

bit size; // 0 = 8-bit data, 1 = 16-bit data

//作者：Thfli
//翻出来以前用51写的访问AT93C46的三个函数，8位的organization。端口输出和输入的语句要按430的用法改一下。

void EEPROM_Write_Enable(void)
{
	//give EWEN command to 93c46, allowing write of 93c46
	unsigned char i;
	EEPROM_CS = 0;
	EEPROM_CLK = 0;
	EEPROM_CS = 1;
	for(i=0;i<3;i++)
	{
		EEPROM_DI = (0x80>>(7-i))&1;//)?1:0;
		EEPROM_CLK = 1; 
		EEPROM_CLK = 0;
	}
	
	for(i=0;i<7;i++)
	{
		EEPROM_DI = (0xc0>>(7-i))&1;//)?1:0;
		EEPROM_CLK = 1; 
		EEPROM_CLK = 0;
	}
	
	EEPROM_CLK = 0;
	EEPROM_CLK = 1; 
	EEPROM_CLK = 0;
	EEPROM_DI = 0;
	EEPROM_CS = 0;
}

void EEPROM_Write(unsigned char Addr,unsigned char Data)
{
	//write to 93c46, only the lower 8 bits of the address will be written
	unsigned char i;
	Addr &= 0x3f;
	Addr |= 0x40;
	EEPROM_CS = 0;
	EEPROM_DI = 0;
	EEPROM_CS = 1;
	EEPROM_DI = 1;
	EEPROM_CLK = 1;
	EEPROM_CLK = 0;
	for(i=0;i<8;i++)
	{
		EEPROM_DI = (Addr>>(7-i))&1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 0;
	}
	for(i=0;i<8;i++)
	{
		EEPROM_CLK = 1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 0;
	}
	for(i=0;i<8;i++)
	{
		EEPROM_DI = (Data>>(7-i))&1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 0;
	}
	EEPROM_DI = 0;
	EEPROM_CS = 0;
	EEPROM_CS = 1;
	while(!EEPROM_DO);
	EEPROM_CS = 0;
}

unsigned char EEPROM_Read(unsigned char Addr)
{
	//read 93c46, only the lower 8 bits of the address will be output
	unsigned char i;
	unsigned char Data=0;
	Addr &= 0x3f;
	Addr |= 0x80;
	EEPROM_DI = 0;
	EEPROM_CLK = 0;
	EEPROM_CS = 1;
	EEPROM_DI = 1;
	EEPROM_CLK = 1;
	EEPROM_CLK=0;
	for(i=0;i<8;i++)
	{
		EEPROM_DI = (Addr>>(7-i))&1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 1;
		EEPROM_CLK = 0;
	}
	for(i=0;i<8;i++)
	{
		EEPROM_CLK = 1;
		EEPROM_CLK=0;
	}
	for(i=0;i<8;i++)
	{ 
		EEPROM_CLK = 1;
		if(EEPROM_DO)
		{ 
			Data |= 1<<(7-i);
		}
		EEPROM_CLK = 0;
	}
	EEPROM_DI = 0;
	EEPROM_CS = 0;
	return Data;
}
