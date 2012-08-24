#include "common.h"

sbit CS=P1^2;
sbit SK=P1^3;
sbit DI=P1^4;
sbit DO=P1^5;

#define AT93CXX_CS_H	(CS = 1)
#define AT93CXX_CS_L	(CS = 0)

#define AT93CXX_SK_H	(SK = 1)
#define AT93CXX_SK_L	(SK = 0)


void shout(uchar num,uchar sdata)
{
    uchar i;
	for(i=0;i<num;i++)
	{
	    AT93CXX_SK_L;//drop clock
		sdata<<=1;//move bit into CY
		DI=CY;//output bit
		_nop_();//delay min 400 nS
		AT93CXX_SK_H;//raise clock
	}
}

void shout2(uchar num,uint sdata)
{
    uchar i;
	sdata<<=7;//note: at93c66
	for(i=0;i<num;i++)
	{
	    AT93CXX_SK_L;
		sdata<<=1;
		DI=CY;
		_nop_();
		AT93CXX_SK_H;
	}
}

void AT93CXX_Write_EN(void)
{
    uchar sb_op=0x80;
	uint add=0x0180;
    AT93CXX_CS_L;
    AT93CXX_SK_H;
    shout(3,sb_op);
	shout2(9,add);
	AT93CXX_CS_L;
}

void AT93CXX_Erase_ALL2(void)
{
    uchar sb_op=0x80;
	uint add=0x0100;
    AT93CXX_CS_L;
    AT93CXX_CS_H;
	shout(3,sb_op);
	shout2(9,add);
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	while(DO==0);
	AT93CXX_CS_L;
} 

uchar AT93CXX_Read(uint rd_add)
{
    uchar i,sb_op=0xc0,rd_data;
	AT93CXX_CS_L;
    AT93CXX_CS_H;
	shout(3,sb_op);
	shout(9,rd_add);
//	SK=0;   //产生一时钟上升沿，将虚拟位0读出
//	SK=1;  	//（虚拟位可能在写A0的时钟上升沿中产生，而不是在写A0后的时钟上升沿产生,所以不用将虚拟位读出)    

	for(i=0;i<8;i++)
	{	rd_data<<=1;
	    AT93CXX_CS_L;
		AT93CXX_SK_H;
		CY=DO;
		if(CY==1)rd_data|=0x01;
		else rd_data&=0xfe;	
	}
	AT93CXX_SK_L;
	AT93CXX_CS_L;
	return(rd_data);
}

void AT93CXX_Write(uint wr_add,uchar wr_data)
{
	uchar sb_op=0xa0;
	AT93CXX_CS_L;
    AT93CXX_CS_H;
	shout(3,sb_op);
	shout2(9,wr_add);
	shout(8,wr_data);
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	while(DO==0);
	AT93CXX_CS_L;
} 

void AT93CXX_Erase(uint er_add)
{
    uchar sb_op=0xe0;
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	shout(3,sb_op);
	shout2(9,er_add);
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	while(DO==0);
	AT93CXX_CS_L;
}


void AT93CXX_Write_ALL2(uchar wr_data)
{
    uchar sb_op=0x80;
	uint wr_add=0x0080;
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	shout(3,sb_op);
	shout2(9,wr_add);
	shout(8,wr_data);
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	while(DO==0);
	AT93CXX_CS_L;
}


void AT93CXX_Disable_EW(void)
{
    uchar sb_op=0x80;
	uint add=0x0000;
	AT93CXX_CS_L;
	AT93CXX_CS_H;
	shout(3,sb_op);
	shout2(9,add);
	AT93CXX_CS_L;
	//delay();
}