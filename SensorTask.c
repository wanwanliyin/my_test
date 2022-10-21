#include <stdio.h>	
#include <string.h>
#include "sys.h"
#include "SensorTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stmflash.h"
#include "rtc.h"
#include "myiic.h"

u8 AHT10ReadStatus(void)//读取AHT10的状态寄存器
{

	u8 Byte_first;	
	I2cStart();
	I2cSendByte(0x71);
	I2cWaitAck();
	Byte_first = I2cReadByte(0);
	
	I2cStop();
	return Byte_first;
}


u8 AHT10ReadCalEnable(void)  //查询cal enable位有没有使能？
{
  u8 val = 0;//ret = 0,
  val = AHT10ReadStatus();
  if((val & 0x68)==0x08)  //判断NOR模式和校准输出是否有效
  {
     return 1;
  }
  else
  {
     return 0;
  }
}

void AHT10SendBA(void)//向AHT10发送BA命令
{
    I2cStart();
    I2cSendByte(0x70);
    I2cWaitAck();
    I2cSendByte(0xba);
    I2cWaitAck();
    I2cStop();
}

void AHT10SendAC(void) //向AHT10发送AC命令
{
    I2cStart();
    I2cSendByte(0x70);
    I2cWaitAck();
    I2cSendByte(0xac);
    I2cWaitAck();
    I2cSendByte(0x33);
    I2cWaitAck();
    I2cSendByte(0x00);
    I2cWaitAck();
    I2cStop();
}

void AHT10ParaInit(void)
{
	I2cStart();
    I2cSendByte(0x70);
    I2cWaitAck();
    I2cSendByte(0xe1);
    I2cWaitAck();
    I2cSendByte(0x08);
    I2cWaitAck();
    I2cSendByte(0x00);
    I2cWaitAck();
    I2cStop();
}

void AHT10Init(void)   //初始化AHT10    未初始化成功会死等
{
    AHT10ParaInit();
	delay_ms(500);//延时0.5S
    while( AHT10ReadCalEnable()==0 )//需要等待状态字status的Bit[3]=1时才去读数据。如果Bit[3]不等于1 ，发软件复位0xBA给AHT10，再重新初始化AHT10，直至Bit[3]=1
    {
       AHT10SendBA();  //复位
       delay_ms(100);
       AHT10ParaInit();
       delay_ms(500);
    }
}

void AHT10ReadCTData(u32 *ct) //读取AHT10的温度和湿度数据
{
	volatile u8  rcvBuf[6] = {0}; //状态字、湿度字节、湿度字节、高4位为湿度  低4位为温度、温度字节、温度字节
    int i;
	u32 RetuData = 0;
	u16 cnt = 0;

	AHT10SendAC();//向AHT10发送AC命令

	delay_ms(75);//等待75ms
    cnt = 0;
	while(((AHT10ReadStatus()&0x80) == 0x80))//等待忙状态结束
	{
 	   delay_ms(2);
	   if(cnt++>=100)
	   {
	      break;
	   }
	}
	I2cStart();

	I2cSendByte(0x71);//0x70+1   0x70为设备地址 1为方向位
	I2cWaitAck();
    for(i=0; i<5; i++)
    {
        rcvBuf[i]= I2cReadByte(1);
    }
    rcvBuf[i]= I2cReadByte(0);

	I2cStop();

	RetuData = (RetuData|rcvBuf[1])<<8;
	RetuData = (RetuData|rcvBuf[2])<<8;
	RetuData = (RetuData|rcvBuf[3]);
	RetuData = RetuData >>4;
	ct[0] = RetuData;
	RetuData = 0;
	RetuData = (RetuData|rcvBuf[3])<<8;
	RetuData = (RetuData|rcvBuf[4])<<8;
	RetuData = (RetuData|rcvBuf[5]);
	RetuData = RetuData&0xfffff;
	ct[1] = RetuData; 

}

void SensorTask(void *pvParameters)
{
    u32 CT_data[2];
	volatile int  cVal,tVal;
    I2cInit();
	delay_ms(10);
    AHT10Init();
    printf("AHT10InitSuccess\r\n");
    while(1)
    {
      AHT10ReadCTData(CT_data);  //读取温度和湿度 ， 可间隔1.5S读一次
	  cVal = CT_data[0]*1000/1024/1024;  //计算得到湿度值（放大了10倍,如果c1=523，表示现在湿度为52.3%）
	  tVal = CT_data[1]*200*10/1024/1024-500;//计算得到温度值（放大了10倍，如果t1=245，表示现在温度为24.5℃）
	  printf("湿度：%d\r\n", cVal);
      printf("温度：%d\r\n", tVal);
      vTaskDelay(2000);	
    }
}



