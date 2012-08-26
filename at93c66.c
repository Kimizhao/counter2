#include "common.h"
#include "at93c66.h"

#include <stdio.h>

// sbit  P1_0  =  P1^0;	 //SCS引脚
// sbit  P1_1  =  P1^1;	 //SCK引脚
// sbit  P1_2  =  P1^2;	 //MOSI引脚
// sbit  P1_3  =  P1^3;	 //MISO引脚
sbit  P1_0  =  P3^4;	 //SCS引脚
sbit  P1_1  =  P2^2;	 //SCK引脚
sbit  P1_2  =  P3^5;	 //MOSI引脚
sbit  P1_3  =  P1^4;	 //MISO引脚

#define AT93CXX_SCS_H     (P1_0 = 1)
#define AT93CXX_SCS_L     (P1_0 = 0)

#define AT93CXX_SCK_H      (P1_1 = 1)
#define AT93CXX_SCK_L      (P1_1 = 0)

#define AT93CXX_MOSI_H     (P1_2 = 1)
#define AT93CXX_MOSI_L     (P1_2 = 0)

#define AT93CXX_MISO       (P1_3)



//*************************************************
//函数名：void AT93CXX_SPI_PORT_INIT( void )
//输入参数：无
//输出参数：无
//功能：初始化AT93CXX端口
//*************************************************
void AT93CXX_SPI_PORT_INIT( void )
{
 AT93CXX_SCS_L;
 AT93CXX_SCS_L;
 AT93CXX_SCK_L;
 AT93CXX_MOSI_L;
}



//*********************************************************
//函数名：void AT93CXX_SPI_Send_Word(unsigned short dat)
//输入参数：发送的16bit数据
//输出参数：无
//功能：通过SPI接口发送16bit数据
//*********************************************************
void AT93CXX_SPI_Send_Word(unsigned short dat)
{
  unsigned short i;
  for(i=0;i!=16;i++)
  {
   if(dat&0x8000) AT93CXX_MOSI_H;
   else  AT93CXX_MOSI_L;

   AT93CXX_SCK_L;
   AT93CXX_SCK_H;
   AT93CXX_SCK_L;
   dat<<=1;
  
  }

}



//*********************************************************
//函数名：void AT93CXX_SPI_Send_Byte(unsigned char dat)
//输入参数：发送的8bit数据
//输出参数：无
//功能：通过SPI接口发送8bit数据
//*********************************************************
void AT93CXX_SPI_Send_Byte(unsigned char dat)
{
  unsigned short i;
  for(i=0;i!=8;i++)
  {
   if(dat&0x80) AT93CXX_MOSI_H;
   else  AT93CXX_MOSI_L;

   AT93CXX_SCK_L;
   AT93CXX_SCK_H;
   AT93CXX_SCK_L;
   dat<<=1;
  
  }

}


//*********************************************************
//函数名：unsigned short AT93CXX_SPI_Rec_Word( )
//输入参数：无
//输出参数：16bit数据
//功能：通过SPI接口接收16bit数据
//*********************************************************
unsigned short AT93CXX_SPI_Rec_Word( )
{
  unsigned short dat;
  unsigned char i;

  for(i=0;i!=16;i++)
  {
   dat<<=1;
   AT93CXX_SCK_L;
   AT93CXX_SCK_H;    
   if(AT93CXX_MISO) dat++;
   AT93CXX_SCK_L;
  
  }
  return dat;
}



//*********************************************************
//函数名：unsigned short AT93CXX_SPI_Rec_Byte( )
//输入参数：无
//输出参数：8bit数据
//功能：通过SPI接口接收8bit数据
//*********************************************************
unsigned short AT93CXX_SPI_Rec_Byte( )
{
  unsigned char dat;
  unsigned char i;

  for(i=0;i!=8;i++)
  {
   dat<<=1;
   AT93CXX_SCK_L;
   AT93CXX_SCK_H;    
   if(AT93CXX_MISO) dat++;
   AT93CXX_SCK_L;  
  }
  return dat;
}




//****************************************************************
//函数名：unsigned short AT93CXX_Read_Data(unsigned short addr)
//输入参数：地址
//输出参数：16bit数据
//功能：读取AT93CXX指定地址的数据
//****************************************************************
unsigned short AT93CXX_Read_Data(unsigned short addr)
{
  unsigned short address;

  AT93CXX_SCS_H;


#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_READ|0x04)<<8) | addr;
#else
//8位数据存储
  address = ((unsigned short)(AT93CXX_READ|0x04)<<9) | addr;
#endif

  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16位数据存储
   address = AT93CXX_SPI_Rec_Word();
#else 
//8位数据存储
   address = AT93CXX_SPI_Rec_Byte();
#endif


   AT93CXX_SCS_L;

   return address;
}





//****************************************************************
//函数名：void  AT93CXX_EN_Write( void )
//输入参数：无
//输出参数：无
//功能：使能写操作和擦除操作
//****************************************************************
void  AT93CXX_EN_Write( void )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据模式
  address = ((unsigned short)(AT93CXX_EWEN|0x04)<<8) | 0xc0;
#else 
//8位数据模式
  address = ((unsigned short)(AT93CXX_EWEN|0x04)<<9) | 0x180;
#endif
  AT93CXX_SPI_Send_Word(address);

  AT93CXX_SCS_L;

}




//****************************************************************
//函数名：void  AT93CXX_Erase_Write_Disable( void )
//输入参数：无
//输出参数：无
//功能：禁止写操作和擦除操作
//****************************************************************
void  AT93CXX_Erase_Write_Disable( void )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_EWDS|0x04)<<8);

#else
//8位数据存储 
  address = ((unsigned short)(AT93CXX_EWDS|0x04)<<9);

#endif
  AT93CXX_SPI_Send_Word(address);

  AT93CXX_SCS_L;
}




//**************************************************************************
//函数名：void  AT93CXX_Write_Data( unsigned short addr,unsigned short dat )
//输入参数：地址，数据
//输出参数：无
//功能：往指定地址写入数据
//****************************************************************************
void  AT93CXX_Write_Data( unsigned short addr,unsigned short dat )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_WRITE|0x04)<<8) | addr;
#else
//8位数据存储 
  address = ((unsigned short)(AT93CXX_WRITE|0x04)<<9)| addr;
#endif
  
  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16位数据存储
    AT93CXX_SPI_Send_Word(dat);
#else 
//8位数据存储
    AT93CXX_SPI_Send_Byte((unsigned char)dat);
#endif
    AT93CXX_SCS_L;
    
	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //等待写完成
    AT93CXX_SCS_L;


}




//**************************************************************************
//函数名：void AT93CXX_Write_All( unsigned short dat)
//输入参数：数据
//输出参数：无
//功能：往所有地址写入固定数据
//****************************************************************************
void AT93CXX_Write_All( unsigned short dat)
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_WRAL|0x04)<<8)|0x40;
#else
//8位数据存储 
  address = ((unsigned short)(AT93CXX_WRAL|0x04)<<9)|0x80;
#endif

  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16位数据存储
  AT93CXX_SPI_Send_Word(dat);

#else 
//8位数据存储
  AT93CXX_SPI_Send_Byte((unsigned char)dat);
#endif
   
    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //等待写完成
    AT93CXX_SCS_L;
}




//**************************************************************************
//函数名：void AT93CXX_Erase_Dat( unsigned short addr)
//输入参数：地址
//输出参数：无
//功能：擦除指定地址数据
//****************************************************************************
void AT93CXX_Erase_Dat( unsigned short addr)
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_ERASE|0x04)<<8) | addr;

#else
//8位数据存储 
  address = ((unsigned short)(AT93CXX_ERASE|0x04)<<9) | addr;
#endif
 
  AT93CXX_SPI_Send_Word(address);

    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //等待擦除完成
    AT93CXX_SCS_L;


}


//**************************************************************************
//函数名：void AT93CXX_Erase_All( )
//输入参数：无
//输出参数：无
//功能：擦除所有地址数据
//****************************************************************************
void AT93CXX_Erase_All( )
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16位数据存储
  address = ((unsigned short)(AT93CXX_ERAL|0x04)<<8) | 0x80;
#else
//8位数据存储 
  address = ((unsigned short)(AT93CXX_ERAL|0x04)<<9) | 0x100;
#endif
  AT93CXX_SPI_Send_Word(address);

    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //等待擦除完成
    AT93CXX_SCS_L;


}









