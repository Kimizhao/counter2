#include "common.h"

unsigned char code arabic[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};/* "0","1","2","3","4","5","6","7","8","9" */
unsigned char code special[]={0x00,0x40,0x08,0x79,0x77,0x7c,0x39,0x5e,0x71,0x3d,0x76};/* " ","-","_","E","A","b","C","d","F","G","H" */
sbit MBI_SDI=P2^5;
sbit MBI_CLK=P2^7;
sbit MBI_LE=P2^6;

#define MBI_BIT_SDI	0x20
#define MBI_BIT_CLK	0x80
#define MBI_BIT_LE		0x40
#define MBI_BIT_OE	0x10

#define Clk_Rising	{MBI_CLK=LOW; MBI_CLK=HIGHT;}
#define LE_HIGH	{MBI_LE=HIGHT;}
#define LE_LOW	{MBI_LE=LOW;}
#define OE_ENABLE	{MBI_OE=LOW;}
#define OE_DISABLE {MBI_OE=HIGHT;}

#define shift_data_1 {MBI_SDI=HIGHT;}
#define shift_data_0 {MBI_SDI=LOW;}

//===================================================
char Few_Digits(unsigned long data2)
{
	if (data2>999)
	{
		if (data2==10000)
		{
			return 5;
		}
		return 4;
	}else if (data2>99)
	{
		return 3;
	}else if (data2>9)
	{
		return 2;
	}else if (data2>=0)
	{
		return 1;
	}
	
	return 0;
}

void MBI_Shift_1_bit(unsigned char lh)
{
	if (lh)
	{
		shift_data_1;
	}else{
		shift_data_0;
	}
	
	Clk_Rising;
}

void MBI_Shift_8_bit(unsigned char Data)
{
	unsigned char i;

	for (i=0;i<8;i++)
	{
		MBI_Shift_1_bit((Data>>(7-i)&1));
	}

}

void MBI_Shift_32_bit(unsigned long datal)
{
	unsigned char i,weishu;
	unsigned char ge,shi,bai,qian;
	ge=arabic[datal%10];
	shi=arabic[(datal/10)%10];
	bai=arabic[(datal/100)%10];
	qian=arabic[(datal/1000)%10];
	weishu=Few_Digits(datal);
	
	
	switch (weishu)
	{
		case 5://全关闭
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			break;
		case 4://4位数
			MBI_Shift_8_bit(qian);
			MBI_Shift_8_bit(bai);
			MBI_Shift_8_bit(shi);
			MBI_Shift_8_bit(ge);
			break;
		case 3://3位数
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(bai);
			MBI_Shift_8_bit(shi);
			MBI_Shift_8_bit(ge);
			break;
		case 2://2位数
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(shi);
			MBI_Shift_8_bit(ge);
			break;
		case 1://1位数
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(special[0]);
			MBI_Shift_8_bit(ge);
			break;
		case 0://溢出
			MBI_Shift_8_bit(special[8]);
			MBI_Shift_8_bit(special[8]);
			MBI_Shift_8_bit(special[8]);
			MBI_Shift_8_bit(special[8]);
			break;
	default:
		break;
	}
	
	LE_HIGH;
	LE_LOW;
}