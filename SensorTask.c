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

u8 AHT10ReadStatus(void)//��ȡAHT10��״̬�Ĵ���
{

	u8 Byte_first;	
	I2cStart();
	I2cSendByte(0x71);
	I2cWaitAck();
	Byte_first = I2cReadByte(0);
	
	I2cStop();
	return Byte_first;
}


u8 AHT10ReadCalEnable(void)  //��ѯcal enableλ��û��ʹ�ܣ�
{
  u8 val = 0;//ret = 0,
  val = AHT10ReadStatus();
  if((val & 0x68)==0x08)  //�ж�NORģʽ��У׼����Ƿ���Ч
  {
     return 1;
  }
  else
  {
     return 0;
  }
}

void AHT10SendBA(void)//��AHT10����BA����
{
    I2cStart();
    I2cSendByte(0x70);
    I2cWaitAck();
    I2cSendByte(0xba);
    I2cWaitAck();
    I2cStop();
}

void AHT10SendAC(void) //��AHT10����AC����
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

void AHT10Init(void)   //��ʼ��AHT10    δ��ʼ���ɹ�������
{
    AHT10ParaInit();
	delay_ms(500);//��ʱ0.5S
    while( AHT10ReadCalEnable()==0 )//��Ҫ�ȴ�״̬��status��Bit[3]=1ʱ��ȥ�����ݡ����Bit[3]������1 ���������λ0xBA��AHT10�������³�ʼ��AHT10��ֱ��Bit[3]=1
    {
       AHT10SendBA();  //��λ
       delay_ms(100);
       AHT10ParaInit();
       delay_ms(500);
    }
}

void AHT10ReadCTData(u32 *ct) //��ȡAHT10���¶Ⱥ�ʪ������
{
	volatile u8  rcvBuf[6] = {0}; //״̬�֡�ʪ���ֽڡ�ʪ���ֽڡ���4λΪʪ��  ��4λΪ�¶ȡ��¶��ֽڡ��¶��ֽ�
    int i;
	u32 RetuData = 0;
	u16 cnt = 0;

	AHT10SendAC();//��AHT10����AC����

	delay_ms(75);//�ȴ�75ms
    cnt = 0;
	while(((AHT10ReadStatus()&0x80) == 0x80))//�ȴ�æ״̬����
	{
 	   delay_ms(2);
	   if(cnt++>=100)
	   {
	      break;
	   }
	}
	I2cStart();

	I2cSendByte(0x71);//0x70+1   0x70Ϊ�豸��ַ 1Ϊ����λ
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
      AHT10ReadCTData(CT_data);  //��ȡ�¶Ⱥ�ʪ�� �� �ɼ��1.5S��һ��
	  cVal = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵ���Ŵ���10��,���c1=523����ʾ����ʪ��Ϊ52.3%��
	  tVal = CT_data[1]*200*10/1024/1024-500;//����õ��¶�ֵ���Ŵ���10�������t1=245����ʾ�����¶�Ϊ24.5�棩
	  printf("ʪ�ȣ�%d\r\n", cVal);
      printf("�¶ȣ�%d\r\n", tVal);
      vTaskDelay(2000);	
    }
}



