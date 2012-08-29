

#include <reg52.h> 
#include <intrins.h>
#include "DS12C887.H"

#define F_ST 0x02
#define F_R 0x72            //帧类型 r 继电器
#define F_W 0x77            //帧类型 w 写counter 
#define F_Q 0x51            //帧类型 Q 询问counter
#define F_q  0x71            //帧类型q 发送当前counter
#define F_END 0x03         //帧尾标志   
#define common 0x33
#define SY 0x30               //不闪
#define NS 0x31     //闪    
#define dian  0x2e  // 小数点
#define F_SPACE 0x20        //空标志             0 

sbit P2A=P2^0;
sbit P2B=P2^1;

//public 变量 
unsigned char D[6]; 
unsigned char qq[]={"OKNG"};
unsigned char CF[]={"CF10"};
unsigned char F_ST1=0x32;          //高位标志           2 
unsigned char F_ST2=0x33;          //低位标志           3 
unsigned long int   counter=978355;
unsigned char R,R1,R2,R3,R4,R5,R6;
unsigned char RSHAN,XSHAN;
unsigned char BR;//波特率
unsigned char code seg[]={0,1,2,3,4,5,6,7};//分别对应相应的数码管点亮
unsigned char const dofly[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// 显示段码值01234567
extern unsigned char Date_Time[7];

//char code str[] = "you are the best! \n\r";
//函数 
//void send(unsigned char ch);
///////////////////////////////////////////////////////////////////// 
//函 数 名：send() 
//功能描述：向串口发送一个字符 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：ch-要发送的ASCII字符 
///////////////////////////////////////////////////////////////////// 
void send(unsigned char ch) 
{SBUF=ch;while(TI==0);TI=0;} 

///////////////////////////////////////////////////////////////////// 
//函 数 名：receive() 
//功能描述：从串口接收一个字符 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：一个ASCII字符 
///////////////////////////////////////////////////////////////////// 
unsigned char receive(void) 
{while(RI==0);RI=0;return SBUF;} 

///////////////////////////////////////////////////////////////////// 
//函 数 名：CharToHex() 
//功能描述：把ASCII字符转换为16进制 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：ASCII字符 
//返    回：16进制 
///////////////////////////////////////////////////////////////////// 
unsigned char CharToHex(unsigned char bChar){ 
    if((bChar>=0x30)&&(bChar<=0x39)) 
        bChar -= 0x30; 
    else if((bChar>=0x41)&&(bChar<=0x46))//大写字母 
        bChar -= 0x37; 
    else if((bChar>=0x61)&&(bChar<=0x66))//小写字母 
        bChar -= 0x57; 
    else bChar = 0xff; 
    return bChar; 
} 

///////////////////////////////////////////////////////////////////// 
//函 数 名：HexToChar() 
//功能描述：把16进制转换为ASCII字符 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：16进制 
//返    回：ASCII字符 
///////////////////////////////////////////////////////////////////// 
unsigned char HexToChar(unsigned char bHex){ 
    if((bHex>=0)&&(bHex<=9)) 
        bHex += 0x30; 
    else if((bHex>=10)&&(bHex<=15))//大写字母 
        bHex += 0x37; 
    else bHex = 0xff; 
    return bHex; 
} 

///////////////////////////////////////////////////////////////////// 
//函 数 名：com_int() 
//功能描述：初始化串口 
//函数说明：默认其他参数为[baud_rate],n,8,1 
//调用函数： 
//全局变量： 
//输    入：baud_rate 波特率 
//返    回：无 
///////////////////////////////////////////////////////////////////// 
void com_init(unsigned char baud_rate){ 
    EA=1;ES=1;//ET1=1;   
    SCON = 0x50;    /* 0x52;//SCON */ 
    TMOD = 0x20;    /*0x20;// TMOD */ 
    TCON = 0x60;    /*0x60;// TCON */ 
    PCON=PCON&0x7f; 
	RI=0;
	TI=0;
    switch(baud_rate){    //波特率设置 
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
//函 数 名：resend_frame() 
//功能描述：发送重发帧 
//函数说明：通知PC重发 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：无 
///////////////////////////////////////////////////////////////////// 
void resend_frame(void){ 
    send(CF[0]);
	send(CF[1]);
} 

///////////////////////////////////////////////////////////////////// 
//函 数 名：quit_frame() 
//功能描述：发送放弃帧 
//函数说明：通知PC放弃通讯 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：无 
///////////////////////////////////////////////////////////////////// 
void quit_frame(void){ 
    send(qq[2]);
	send(qq[3]);

} 
///////////////////////////////////////////////////////////////////// 
//函 数 名：Id_SET() 
//功能描述：ID设定 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：无 
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
//函 数 名：InttoHex() 
//功能描述：整数转换成数组中断 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：无 
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
//函 数 名：HextoInt() 
//功能描述：数组转换成整数中断 
//函数说明： 
//调用函数： 
//全局变量： 
//输    入：无 
//返    回：无 
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
//函 数 名：com_int() 
//功能描述：串口中断 
//函数说明： 
//调用函数： 
//全局变量： 
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
if(receive()==F_ST1){             //是侦 
	
    if(receive()==F_ST2){//头判断完 
     
        //communicationing=1;//设置通讯状态为正常    即启动通讯 
        switch(receive()){ 
            case F_W://是读指令帧r 
				//P1=0X11;
                if(1){
					     //P1=0x04;;//地址正确      
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
						   if(XSHAN==0X30) //继电器不动作 不闪烁
							{
								R5=0;
							//	P1=0x04;
							}
							if(XSHAN==0X31) //动作继电器 2HZ闪烁
							{
							    R5=1;
							//P1=0x08;
							}

                         end=CharToHex(receive())<<4;
						 end+=CharToHex(receive()); 
                        if(end==F_END)
						{//结束标志正确    
						 //P1=0x10;
						 send(qq[0]);
						 send(qq[1]);
						 HextoInt();

                        }//else  {ERR_NOCNTMAX_RESEND break;}//结束标志错误 
                }//地址不正确 
                break; 
            case F_Q://是PC写指令帧w 
				    end=CharToHex(receive())<<4;
					end+=CharToHex(receive()); 
                if(end==F_END)//结束标志正确 
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
                }//从PC获得数据写完毕//开始发送写回应帧 
				//P1=0x12;
                break; 
            case F_R://继电器动作
				R=receive();
				RSHAN=receive();
 				end=CharToHex(receive())<<4;
				end+=CharToHex(receive()); 
                if(end==F_END)//结束标志正确 
				{
					//P1=0x01;
					if(R==0X30)// 选择R1
					{
					R1=1;
				//	P1=0x01;
					}
					if(R==0X31)//选择R2
					{
					R2=1;
					//P1=0x02;
					}
					if(RSHAN==0X30) //继电器不动作 不闪烁
					{
					R3=1;
				//	P1=0x04;
					}
					if(RSHAN==0X31) //动作继电器 2HZ闪烁
					{
					R4=1;
					//P1=0x08;
					}
				}

                break;
            default: 
                resend_frame();//要求从发 
        } 
    }//忽略     
}//忽略 
//if(!communicationing)quit_frame();//调用放弃帧，通知PC 放弃通讯 
}
}

else{ TI = 0; }

} 
///////////////////////////////////////////////////////////////// 

//主函数 

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
	   P0=dofly[(Date_Time[6]/10)];//取显示数据
	   P2=seg[0];  //取段码
	   delay(5); //扫描间隙延时
	   P0=dofly[(Date_Time[6]%10)];//取显示数据
	   P2=seg[1];  //取段码
	   delay(5); //扫描间隙延时
	   P0=dofly[(Date_Time[5]/10)];//取显示数据
	   P2=seg[2];  //取段码
	   delay(5); //扫描间隙延时
	   P0=dofly[(Date_Time[5]%10)];//取显示数据
	   P2=seg[3];  //取段码
	   delay(5); //扫描间隙延时
	   P0=dofly[(Date_Time[4]/10)];//取显示数据
	   P2=seg[4];  //取段码
	   delay(5); //扫描间隙延时
	   P0=dofly[(Date_Time[4]%10)];//取显示数据
	   P2=seg[5];  //取段码
	   delay(5); //扫描间隙延时	   	  	   
}
} 



