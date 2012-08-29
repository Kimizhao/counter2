

#include <reg52.h> 
#include <intrins.h>
#include "DS12C887.H"

#define F_ST 0x02
#define F_R 0x72            //֡���� r �̵���
#define F_W 0x77            //֡���� w дcounter 
#define F_Q 0x51            //֡���� Q ѯ��counter
#define F_q  0x71            //֡����q ���͵�ǰcounter
#define F_END 0x03         //֡β��־   
#define common 0x33
#define SY 0x30               //����
#define NS 0x31     //��    
#define dian  0x2e  // С����
#define F_SPACE 0x20        //�ձ�־             0 

sbit P2A=P2^0;
sbit P2B=P2^1;

//public ���� 
unsigned char D[6]; 
unsigned char qq[]={"OKNG"};
unsigned char CF[]={"CF10"};
unsigned char F_ST1=0x32;          //��λ��־           2 
unsigned char F_ST2=0x33;          //��λ��־           3 
unsigned long int   counter=978355;
unsigned char R,R1,R2,R3,R4,R5,R6;
unsigned char RSHAN,XSHAN;
unsigned char BR;//������
unsigned char code seg[]={0,1,2,3,4,5,6,7};//�ֱ��Ӧ��Ӧ������ܵ���
unsigned char const dofly[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// ��ʾ����ֵ01234567
extern unsigned char Date_Time[7];

//char code str[] = "you are the best! \n\r";
//���� 
//void send(unsigned char ch);
///////////////////////////////////////////////////////////////////// 
//�� �� ����send() 
//�����������򴮿ڷ���һ���ַ� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺ch-Ҫ���͵�ASCII�ַ� 
///////////////////////////////////////////////////////////////////// 
void send(unsigned char ch) 
{SBUF=ch;while(TI==0);TI=0;} 

///////////////////////////////////////////////////////////////////// 
//�� �� ����receive() 
//�����������Ӵ��ڽ���һ���ַ� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ�һ��ASCII�ַ� 
///////////////////////////////////////////////////////////////////// 
unsigned char receive(void) 
{while(RI==0);RI=0;return SBUF;} 

///////////////////////////////////////////////////////////////////// 
//�� �� ����CharToHex() 
//������������ASCII�ַ�ת��Ϊ16���� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺ASCII�ַ� 
//��    �أ�16���� 
///////////////////////////////////////////////////////////////////// 
unsigned char CharToHex(unsigned char bChar){ 
    if((bChar>=0x30)&&(bChar<=0x39)) 
        bChar -= 0x30; 
    else if((bChar>=0x41)&&(bChar<=0x46))//��д��ĸ 
        bChar -= 0x37; 
    else if((bChar>=0x61)&&(bChar<=0x66))//Сд��ĸ 
        bChar -= 0x57; 
    else bChar = 0xff; 
    return bChar; 
} 

///////////////////////////////////////////////////////////////////// 
//�� �� ����HexToChar() 
//������������16����ת��ΪASCII�ַ� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺16���� 
//��    �أ�ASCII�ַ� 
///////////////////////////////////////////////////////////////////// 
unsigned char HexToChar(unsigned char bHex){ 
    if((bHex>=0)&&(bHex<=9)) 
        bHex += 0x30; 
    else if((bHex>=10)&&(bHex<=15))//��д��ĸ 
        bHex += 0x37; 
    else bHex = 0xff; 
    return bHex; 
} 

///////////////////////////////////////////////////////////////////// 
//�� �� ����com_int() 
//������������ʼ������ 
//����˵����Ĭ����������Ϊ[baud_rate],n,8,1 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺baud_rate ������ 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 
void com_init(unsigned char baud_rate){ 
    EA=1;ES=1;//ET1=1;   
    SCON = 0x50;    /* 0x52;//SCON */ 
    TMOD = 0x20;    /*0x20;// TMOD */ 
    TCON = 0x60;    /*0x60;// TCON */ 
    PCON=PCON&0x7f; 
	RI=0;
	TI=0;
    switch(baud_rate){    //���������� 
        case 1: 
            TL1=0xe8;TH1=0Xe8;break;//1200 
        case 2: 
            TL1=0xf4;TH1=0Xf4;break;//2400 
        case 4: 
            TL1=0xfa;TH1=0Xfa;break;//4800 
        case 8: 
		//	P1=0X12;
            TL1=0xfd;TH1=0Xfd;break;//9600 
        case 16: 
            PCON=PCON|0x80;TL1=0xfd;TH1=0Xfd;break;//19200 
        case 32: 
            PCON=PCON|0x80;TL1=0xfe;TH1=0Xfe;break;//38400 
        default: 
             TL1=0xfd;TH1=0Xfd;break;//9600 
    } 
} 
///////////////////////////////////////////////////////////////////// 
//�� �� ����resend_frame() 
//���������������ط�֡ 
//����˵����֪ͨPC�ط� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 
void resend_frame(void){ 
    send(CF[0]);
	send(CF[1]);
} 

///////////////////////////////////////////////////////////////////// 
//�� �� ����quit_frame() 
//�������������ͷ���֡ 
//����˵����֪ͨPC����ͨѶ 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 
void quit_frame(void){ 
    send(qq[2]);
	send(qq[3]);

} 
///////////////////////////////////////////////////////////////////// 
//�� �� ����Id_SET() 
//����������ID�趨 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 
void Id_SET()
{
P2A=0;
P2B=1;
F_ST1=(P0&0xf0);
F_ST1=F_ST1>>4;
F_ST1=HexToChar(F_ST1);
//P1=F_ST1;
F_ST2=(P0&0x0f);
//P1=F_ST2;
F_ST2=HexToChar(F_ST2);
P2A=1;
P2B=1;
BR=P0;
P2A=1;
P2B=0;
}

///////////////////////////////////////////////////////////////////// 
//�� �� ����InttoHex() 
//��������������ת���������ж� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 

void InttoHex(unsigned long int count)
{
D[0]=count/100000;
D[1]=(count%100000)/10000;
D[2]=(count%10000)/1000;
D[3]=(count%1000)/100;
D[4]=(count%100)/10;
D[5]=(count%10);
}

///////////////////////////////////////////////////////////////////// 
//�� �� ����HextoInt() 
//��������������ת���������ж� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
//��    �룺�� 
//��    �أ��� 
///////////////////////////////////////////////////////////////////// 
void HextoInt()
{

unsigned char i;
counter=D[0];
for(i=0;i<5;i++)
{
counter=counter*10+D[1+i];
}

}

///////////////////////////////////////////////////////////////////// 
//�� �� ����com_int() 
//���������������ж� 
//����˵���� 
//���ú����� 
//ȫ�ֱ����� 
///////////////////////////////////////////////////////////////////// 
void com_int()interrupt 4{ 
unsigned char i,csum,st,mici,end; 
csum=0; 
 
if(RI == 1){
RI = 0;
st=CharToHex(SBUF)<<4;
st+=CharToHex(receive()); 
if(st==F_ST){
//P1=receive();
if(receive()==F_ST1){             //���� 
	
    if(receive()==F_ST2){//ͷ�ж��� 
     
        //communicationing=1;//����ͨѶ״̬Ϊ����    ������ͨѶ 
        switch(receive()){ 
            case F_W://�Ƕ�ָ��֡r 
				//P1=0X11;
                if(1){
					     //P1=0x04;;//��ַ��ȷ      
						  for(i=0;i<7;i++)
						  {
						     if(receive()==0x2e)
							 {
								mici=i;
								D[i]=0x2e;
							 }
							 else
							 {
								D[i]=CharToHex(SBUF);
							 }
						  }
                  
                           XSHAN=receive();
						   if(XSHAN==0X30) //�̵��������� ����˸
							{
								R5=0;
							//	P1=0x04;
							}
							if(XSHAN==0X31) //�����̵��� 2HZ��˸
							{
							    R5=1;
							//P1=0x08;
							}

                         end=CharToHex(receive())<<4;
						 end+=CharToHex(receive()); 
                        if(end==F_END)
						{//������־��ȷ    
						 //P1=0x10;
						 send(qq[0]);
						 send(qq[1]);
						 HextoInt();

                        }//else  {ERR_NOCNTMAX_RESEND break;}//������־���� 
                }//��ַ����ȷ 
                break; 
            case F_Q://��PCдָ��֡w 
				    end=CharToHex(receive())<<4;
					end+=CharToHex(receive()); 
                if(end==F_END)//������־��ȷ 
				{
				InttoHex(counter);
				send(HexToChar(((F_ST&0xf0)>>4)));
				send(HexToChar((F_ST&0x0f)));
				send(F_ST1);
				send(F_ST2);
				send(F_q);
				send(HexToChar(D[0]));
				send(HexToChar(D[1]));
				send(HexToChar(D[2]));
				send(HexToChar(D[3]));
				send(HexToChar(D[4]));
				send(HexToChar(D[5]));
                send(D[6]);
				if(R5==1){
				send(CF[2]);
			   	}
				if(R5==0){send(CF[3]);}
				send(HexToChar(((F_END&0xf0)>>4)));
				send(HexToChar((F_END&0x0f)));
                }//��PC�������д���//��ʼ����д��Ӧ֡ 
				//P1=0x12;
                break; 
            case F_R://�̵�������
				R=receive();
				RSHAN=receive();
 				end=CharToHex(receive())<<4;
				end+=CharToHex(receive()); 
                if(end==F_END)//������־��ȷ 
				{
					//P1=0x01;
					if(R==0X30)// ѡ��R1
					{
					R1=1;
				//	P1=0x01;
					}
					if(R==0X31)//ѡ��R2
					{
					R2=1;
					//P1=0x02;
					}
					if(RSHAN==0X30) //�̵��������� ����˸
					{
					R3=1;
				//	P1=0x04;
					}
					if(RSHAN==0X31) //�����̵��� 2HZ��˸
					{
					R4=1;
					//P1=0x08;
					}
				}

                break;
            default: 
                resend_frame();//Ҫ��ӷ� 
        } 
    }//����     
}//���� 
//if(!communicationing)quit_frame();//���÷���֡��֪ͨPC ����ͨѶ 
}
}

else{ TI = 0; }

} 
///////////////////////////////////////////////////////////////// 

//������ 

///////////////////////////////////////////////////////////////// 
void main(void){ 
//Id_SET();
com_init(8);
//P1=0x32; 
//send(F_ST1);
WriteDs12887();


while(1)
{
       ReadDs127887();
	   P0=dofly[(Date_Time[6]/10)];//ȡ��ʾ����
	   P2=seg[0];  //ȡ����
	   delay(5); //ɨ���϶��ʱ
	   P0=dofly[(Date_Time[6]%10)];//ȡ��ʾ����
	   P2=seg[1];  //ȡ����
	   delay(5); //ɨ���϶��ʱ
	   P0=dofly[(Date_Time[5]/10)];//ȡ��ʾ����
	   P2=seg[2];  //ȡ����
	   delay(5); //ɨ���϶��ʱ
	   P0=dofly[(Date_Time[5]%10)];//ȡ��ʾ����
	   P2=seg[3];  //ȡ����
	   delay(5); //ɨ���϶��ʱ
	   P0=dofly[(Date_Time[4]/10)];//ȡ��ʾ����
	   P2=seg[4];  //ȡ����
	   delay(5); //ɨ���϶��ʱ
	   P0=dofly[(Date_Time[4]%10)];//ȡ��ʾ����
	   P2=seg[5];  //ȡ����
	   delay(5); //ɨ���϶��ʱ	   	  	   
}
} 



