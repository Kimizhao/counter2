#include "common.h"

sbit MBI_SDI=P2^5;
sbit MBI_CLK=P2^7;
sbit MBI_LE=P2^6;

void SerialDataOutput(unsigned char Data)
{
	unsigned char i;
	MBI_LE=LOW;
	MBI_CLK=LOW;
	MBI_SDI=LOW;
	
	for (i=0;i<8;i++)
	{
		MBI_SDI=(Data>>(7-i))&1;
		MBI_CLK=HIGHT;
		//MBI_CLK=HIGHT;
		_nop_();
		//_nop_();
		MBI_CLK=LOW;
	}
	
	MBI_LE=HIGHT;
	_nop_();
	//_nop_();
	MBI_LE=LOW;
}