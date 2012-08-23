#include "common.h"

sbit CS=P2^0;
sbit SK=P2^1;
sbit DI=P2^2;
sbit DO=P2^3;

void delay(void)
{
    uint i;
	for(i=0;i<60000;i++);
}

/*注意：由于写入AT93C46的地址只为7位，而我们传送到AT93C46的地址为8位16进制数，故需先
将最高位移出，再将其他7位传送到AT93C46中*/
void shift1(uchar num,uchar sdata)
{
    uchar i;
	for(i=0;i<num;i++)
	{
	    SK=0;
		sdata<<=1;
		DI=CY;
		SK=1;
	}
}

/*由于要传送的地址为8位16进制数，而AT93C46的地址只为7位，故需先将sdata的最高位移出，
再传送到AT93C46中*/
void shift2(uchar num,uchar sdata)
{
    uchar i;
	sdata<<=1;
	for(i=0;i<num;i++)
	{
	    SK=0;
		sdata<<=1;
		DI=CY;
		SK=1;
	}
}
void write_en(void)
{
    uchar sb_op=0x80,add=0xc0;
    CS=0;
    CS=1;
    shift1(3,sb_op);
	shift1(7,add);
	CS=0;
	CS=1;
}

void erase_all(void)
{
    uchar sb_op=0x80,add=0x80;
    CS=0;
    CS=1;
	shift1(3,sb_op);
	shift1(7,add);
	CS=0;
	CS=1;
	while(DO==0);
} 

uchar read(uchar rd_add)
{
    uchar i,sb_op=0xc0,rd_data=0x00;
	CS=0;
    CS=1;
	shift1(3,sb_op);
	shift2(7,rd_add);
//	SK=0;   //产生一时钟上升沿，将虚拟位0读出
//	SK=1;  	//（虚拟位可能在写A0的时钟上升沿中产生，而不是在写A0后的时钟上升沿产生,所以不用将虚拟位读出)           
	for(i=0;i<8;i++)
	{	rd_data<<=1;
	    SK=0;
		SK=1;
		CY=DO;
		if(CY==1)rd_data|=0x01;
		else rd_data&=0xfe;	
	}
	//SK=0;
	CS=0;
	CS=1;
	return(rd_data);
}

void write(uchar wr_add,uchar wr_data)
{
	uchar sb_op=0xa0;
	CS=0;
    CS=1;
	shift1(3,sb_op);
	shift2(7,wr_add);
	shift1(8,wr_data);
	CS=0;
	CS=1;
	while(DO==0);
} 

void erase(uchar er_add)
{
    uchar sb_op=0xe0;
	CS=0;
	CS=1;
	shift1(3,sb_op);
	shift2(7,er_add);
	CS=0;
	CS=1;
	while(DO==0);
}

void wr_all(uchar wr_data)
{
    uchar sb_op=0x80,wr_add=0x40;
	CS=0;
	CS=1;
	shift1(3,sb_op);
	shift1(7,wr_add);
	shift1(8,wr_data);
	CS=0;
	CS=1;
	while(DO==0);
}

void ew_disable(void)
{
    uchar sb_op=0x80,add=0x00;
	CS=0;
	CS=1;
	shift1(3,sb_op);
	shift1(7,add);
	CS=0;
	delay();
}

void main(void)
{	
   	uchar address=0x00,i;
	uchar data1[]={0xfe,0xfd,0xfb,0xf7};
    write_en();
	write(0x00,data1[0]);
	P1=data1[0];
	delay();
	write(0x01,data1[1]);
	P1=data1[1];
	delay();
	write(0x02,data1[2]);
	P1=data1[2];
	delay();
	write(0x03,data1[3]);
	P1=data1[3];
	delay();

	/*
	for(i=0;i<4;i++)
	{  P1=data1[i];
	   write(address,data1[i]);
	   address++;
	   delay();
	} */
   	P1=read(0x00);
	delay();
	P1=read(0x01);
	delay();
	P1=read(0x02);
	delay();
	P1=read(0x03);
	delay();



/*	address=0x00;
	for(i=0;i<4;i++)
	{
	   P1=read(address);
	   delay(); 
	   address++;
	 } */
	while(1);
}