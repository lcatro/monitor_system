
#include <reg52.h> 	
#include <intrins.h>	

#include "device_eeprom.h"

typedef enum {	
	true=1,
	false=0,
} bool;

sbit scl=P3^6;		//总线连接口定义
sbit sda=P3^7;		//总线连接口定义

void delayus() 	//需要4个机器周期,大概4.34us
{
	;					//晶振频率11.0592M,机器周期为1.085微秒
}

void delayms(unsigned int ms)  //软延时函数
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
	{
        for(j=113;j>0;j--)
        {}
	}
}

void iic_start()  //启动信号
{
	sda=1;
	scl=1;
	delayus();		//sda和scl同为高电平保持4.7us以上
	_nop_();			//1.085us,共5.78us,下面sda=0是下降沿,不能计算在延时时间中
	sda=0; 			//下降沿
	delayus();		//sda低电平保持4us以上	,这里是4.34us满足要求
}

void iic_stop()	//停止信号
{
	sda=0;_nop_();	//准备状态
	scl=1;
	delayus();		//该状态稳定时间要求保持4us以上
	sda=1;			//scl高电平期间,sda来一个上升沿
	delayus();		//sda保持4.7us以上,4.34加上函数返回时间大于4.7us
						//注:此时scl和sda都为1	
}

void iic_sendByte(unsigned char byteData) //mcu发送一个字节
{
	unsigned int i;
	unsigned char temp=byteData;
	for(i=0;i<8;i++)
	{
		temp=temp<<1;   //移动后最高位到了PSW寄存器的CY位中
		scl=0;			 //准备
		_nop_();		    //稳定一下
		sda=CY;			 //将待发送的数据一位位的放到sda上
		_nop_();
		scl=1;    		 //每一个高电平期间,ic器件都会将数据取走
		_nop_();		
	}

	scl=0;				//如果写成scl=1;sda=1就是停止信号,不能这么写
	_nop_();				
	sda=1;				//释放总线,数据总线不用时要释放
	_nop_();
}

unsigned char iic_readByte() //读一个字节
{
	unsigned char i,temp;
	scl=0;				//准备读数据
	_nop_();
	sda=1;				//释放总线
	_nop_();

	for(i=0;i<8;i++)
	{
		scl=1;			//mcu开始取数据
		delayus();		//scl为高电平后,ic器件就会将1位数据送到sda上
							//总共用时不会大于4.34us的,然后就可以让mcu读sda了
		temp=(temp<<1)|sda; //读一位保存到temp中
		scl=0;
		delayus();		
	}
	return temp;
}

bool iic_checkACK()		//处理应答信号
{
	unsigned char errCounts=255; //定义超时量为255次
	scl=1;
	_nop_();
	
	while(sda)
	{	//在一段时间内检测到sda=0的话认为是应答信号
		if(0==errCounts)
		{
			scl=0;		  //钳住总线
			_nop_();
			return false; //没有应答信号
		}
		errCounts--;
	}

	scl=0;			  //钳住总线,为下1次通信做准备 
	_nop_();
	return true;	  //成功处理应答信号
}

void AT24C02_init()	//总线初始化
{
	scl=1;
	sda=1;
	delayus();
}

void iic_sendACK(bool b_ACK)	//发送应答或非应答信号
{
	scl=0;			//准备
	_nop_();

	if(b_ACK)		//ACK
	{
		sda=0;
	}
	else			   //unACK
	{
		sda=1;
	}

	_nop_();
	scl=1;
	delayus(); 		//大于4us的延时
	scl=0;  	  		//钳住scl,以便继续接收数据	
	_nop_();
}


void AT24C02_writeByte(unsigned char address,unsigned char dataByte)//向24c02写一字节数据
{
	iic_start();
	iic_sendByte(0xa0);//mcu写控制字,前4位固定1010,后三位地址0,末位0是写
	iic_checkACK();		   //mcu处理应答信号
	iic_sendByte(address);  //准备在指定地址处写入	
	iic_checkACK();
	iic_sendByte(dataByte); //写数据
	iic_checkACK();
	iic_stop();
	delayms(2);	
	//按字节写入时,24c02在接收到停止信号后将数据擦写到内部,这需要时间
	//并且在这段时间内不会响应总线上的任何请求,故让mcu有2毫秒以上的等待	
}

void AT24C02_writeData(unsigned char address,unsigned char numBytes,unsigned char* buf)//写入任意长度数据
{
	while(numBytes--)
	{
		AT24C02_writeByte(address++,*buf++);
	}
}

void AT24C02_readData(unsigned char beginAddr,unsigned char dataSize,unsigned char* buf)//读取任意长度字节
{
	iic_start();					//起始信号
	iic_sendByte(0xa0);			//控制字,写
	iic_checkACK();				//处理应答信号
	iic_sendByte(beginAddr);	//发送地址
	iic_checkACK();				//处理应答信号	
	iic_start();			   	//发送起始信号
	iic_sendByte(0xa1);			//控制字,读
	iic_checkACK();				//处理应答信号
	while(dataSize--)				//读取dataSize个字节
	{
		*buf++=iic_readByte();	//读取一个个字节并保存到缓冲区buf中
		iic_sendACK(dataSize);  //发送应答,当dataSize为0时发送非应答
	}
	iic_stop();						//发送停止信号
}