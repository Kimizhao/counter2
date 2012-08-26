#include "common.h"
#include "at93c66.h"

#include <stdio.h>

// sbit  P1_0  =  P1^0;	 //SCS����
// sbit  P1_1  =  P1^1;	 //SCK����
// sbit  P1_2  =  P1^2;	 //MOSI����
// sbit  P1_3  =  P1^3;	 //MISO����
sbit  P1_0  =  P3^4;	 //SCS����
sbit  P1_1  =  P2^2;	 //SCK����
sbit  P1_2  =  P3^5;	 //MOSI����
sbit  P1_3  =  P1^4;	 //MISO����

#define AT93CXX_SCS_H     (P1_0 = 1)
#define AT93CXX_SCS_L     (P1_0 = 0)

#define AT93CXX_SCK_H      (P1_1 = 1)
#define AT93CXX_SCK_L      (P1_1 = 0)

#define AT93CXX_MOSI_H     (P1_2 = 1)
#define AT93CXX_MOSI_L     (P1_2 = 0)

#define AT93CXX_MISO       (P1_3)



//*************************************************
//��������void AT93CXX_SPI_PORT_INIT( void )
//�����������
//�����������
//���ܣ���ʼ��AT93CXX�˿�
//*************************************************
void AT93CXX_SPI_PORT_INIT( void )
{
 AT93CXX_SCS_L;
 AT93CXX_SCS_L;
 AT93CXX_SCK_L;
 AT93CXX_MOSI_L;
}



//*********************************************************
//��������void AT93CXX_SPI_Send_Word(unsigned short dat)
//������������͵�16bit����
//�����������
//���ܣ�ͨ��SPI�ӿڷ���16bit����
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
//��������void AT93CXX_SPI_Send_Byte(unsigned char dat)
//������������͵�8bit����
//�����������
//���ܣ�ͨ��SPI�ӿڷ���8bit����
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
//��������unsigned short AT93CXX_SPI_Rec_Word( )
//�����������
//���������16bit����
//���ܣ�ͨ��SPI�ӿڽ���16bit����
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
//��������unsigned short AT93CXX_SPI_Rec_Byte( )
//�����������
//���������8bit����
//���ܣ�ͨ��SPI�ӿڽ���8bit����
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
//��������unsigned short AT93CXX_Read_Data(unsigned short addr)
//�����������ַ
//���������16bit����
//���ܣ���ȡAT93CXXָ����ַ������
//****************************************************************
unsigned short AT93CXX_Read_Data(unsigned short addr)
{
  unsigned short address;

  AT93CXX_SCS_H;


#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_READ|0x04)<<8) | addr;
#else
//8λ���ݴ洢
  address = ((unsigned short)(AT93CXX_READ|0x04)<<9) | addr;
#endif

  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16λ���ݴ洢
   address = AT93CXX_SPI_Rec_Word();
#else 
//8λ���ݴ洢
   address = AT93CXX_SPI_Rec_Byte();
#endif


   AT93CXX_SCS_L;

   return address;
}





//****************************************************************
//��������void  AT93CXX_EN_Write( void )
//�����������
//�����������
//���ܣ�ʹ��д�����Ͳ�������
//****************************************************************
void  AT93CXX_EN_Write( void )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ����ģʽ
  address = ((unsigned short)(AT93CXX_EWEN|0x04)<<8) | 0xc0;
#else 
//8λ����ģʽ
  address = ((unsigned short)(AT93CXX_EWEN|0x04)<<9) | 0x180;
#endif
  AT93CXX_SPI_Send_Word(address);

  AT93CXX_SCS_L;

}




//****************************************************************
//��������void  AT93CXX_Erase_Write_Disable( void )
//�����������
//�����������
//���ܣ���ֹд�����Ͳ�������
//****************************************************************
void  AT93CXX_Erase_Write_Disable( void )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_EWDS|0x04)<<8);

#else
//8λ���ݴ洢 
  address = ((unsigned short)(AT93CXX_EWDS|0x04)<<9);

#endif
  AT93CXX_SPI_Send_Word(address);

  AT93CXX_SCS_L;
}




//**************************************************************************
//��������void  AT93CXX_Write_Data( unsigned short addr,unsigned short dat )
//�����������ַ������
//�����������
//���ܣ���ָ����ַд������
//****************************************************************************
void  AT93CXX_Write_Data( unsigned short addr,unsigned short dat )
{
  unsigned short address;

  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_WRITE|0x04)<<8) | addr;
#else
//8λ���ݴ洢 
  address = ((unsigned short)(AT93CXX_WRITE|0x04)<<9)| addr;
#endif
  
  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16λ���ݴ洢
    AT93CXX_SPI_Send_Word(dat);
#else 
//8λ���ݴ洢
    AT93CXX_SPI_Send_Byte((unsigned char)dat);
#endif
    AT93CXX_SCS_L;
    
	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //�ȴ�д���
    AT93CXX_SCS_L;


}




//**************************************************************************
//��������void AT93CXX_Write_All( unsigned short dat)
//�������������
//�����������
//���ܣ������е�ַд��̶�����
//****************************************************************************
void AT93CXX_Write_All( unsigned short dat)
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_WRAL|0x04)<<8)|0x40;
#else
//8λ���ݴ洢 
  address = ((unsigned short)(AT93CXX_WRAL|0x04)<<9)|0x80;
#endif

  AT93CXX_SPI_Send_Word(address);

#if MEM_ORG  
//16λ���ݴ洢
  AT93CXX_SPI_Send_Word(dat);

#else 
//8λ���ݴ洢
  AT93CXX_SPI_Send_Byte((unsigned char)dat);
#endif
   
    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //�ȴ�д���
    AT93CXX_SCS_L;
}




//**************************************************************************
//��������void AT93CXX_Erase_Dat( unsigned short addr)
//�����������ַ
//�����������
//���ܣ�����ָ����ַ����
//****************************************************************************
void AT93CXX_Erase_Dat( unsigned short addr)
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_ERASE|0x04)<<8) | addr;

#else
//8λ���ݴ洢 
  address = ((unsigned short)(AT93CXX_ERASE|0x04)<<9) | addr;
#endif
 
  AT93CXX_SPI_Send_Word(address);

    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //�ȴ��������
    AT93CXX_SCS_L;


}


//**************************************************************************
//��������void AT93CXX_Erase_All( )
//�����������
//�����������
//���ܣ��������е�ַ����
//****************************************************************************
void AT93CXX_Erase_All( )
{
  unsigned short address;
  AT93CXX_SCS_H;

#if MEM_ORG  
//16λ���ݴ洢
  address = ((unsigned short)(AT93CXX_ERAL|0x04)<<8) | 0x80;
#else
//8λ���ݴ洢 
  address = ((unsigned short)(AT93CXX_ERAL|0x04)<<9) | 0x100;
#endif
  AT93CXX_SPI_Send_Word(address);

    AT93CXX_SCS_L;

	AT93CXX_SCS_H;
    while(AT93CXX_MISO == 0); //�ȴ��������
    AT93CXX_SCS_L;


}









