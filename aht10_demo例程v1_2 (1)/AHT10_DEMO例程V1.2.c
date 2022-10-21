/*******************************************/
/*@��Ȩ���У����ݰ��ɵ������޹�˾          */
/*@���ߣ���ʪ�ȴ�������ҵ��                */
/*@�汾��V1.2                              */
/*******************************************/
/*******************************************/
/*@�汾˵����                              */
/*@�汾�ţ�V1.2 �޸�AC����Ĳ�����         */
/*@�汾�ţ�V1.1 ����У׼���ʹ�ܼ�⡣     */
/*@�汾�ţ�V1.0 ����汾��                 */
/*******************************************/



void Delay_N10us(uint32_t t)//��ʱ����
{
  uint32_t k;

   while(t--)
  {
    for (k = 0; k < 2; k++)//110
      ;
  }
}



void SensorDelay_us(uint32_t t)//��ʱ����
{
	t = t-2;	
	for(; t>0; t--)
	{
		Delay_N10us(1);
	}

}

void Delay_3us(void)		//��ʱ����
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}
void Delay_1us(void)		//��ʱ����
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);

}


void SDA_Pin_Output_High(void)   //��P15����Ϊ��� �� ������Ϊ�ߵ�ƽ�� P15��ΪI2C��SDA
{

 Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
 Gpio_SetIO(1,5,1); 
}

void SDA_Pin_Output_Low(void)  //��P15����Ϊ���  ������Ϊ�͵�ƽ
{
 Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
 Gpio_SetIO(1,5,0);  
}

void SDA_Pin_IN_FLOATING(void)  //SDA����Ϊ��������
{
 Gpio_InitIO(1, 5, GpioDirIn);
}



void SCL_Pin_Output_High(void) //SCL����ߵ�ƽ��P14��ΪI2C��SCL
{
 Gpio_SetIO(1, 4, 1);
}

void SCL_Pin_Output_Low(void) //SCL����͵�ƽ
{
 Gpio_SetIO(1, 4, 0);
}

void Init_I2C_Sensor_Port(void) //��ʼ��I2C�ӿ�
{	

 Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//��P15����Ϊ��©���  ������Ϊ�ߵ�ƽ
 Gpio_SetIO(1,5,1);
	

 Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//��P14����Ϊ��©���  ������Ϊ�ߵ�ƽ
 Gpio_SetIO(1,4,1);
	
}


void I2C_Start(void)		 //I2C��������START�ź�
{
 SDA_Pin_Output_High();
 SensorDelay_us(8);
 SCL_Pin_Output_High();
 SensorDelay_us(8);
 SDA_Pin_Output_Low();
 SensorDelay_us(8);
 SCL_Pin_Output_Low();
 SensorDelay_us(8);   
}


void ZSSC_I2C_WR_Byte(uint8_t Byte) //��AHT10дһ���ֽ�
{
	uint8_t Data,N,i;	
	Data=Byte;
	i = 0x80;
	for(N=0;N<8;N++)
	{
		SCL_Pin_Output_Low();
	  
		Delay_3us();	
		
		if(i&Data)
		{
			SDA_Pin_Output_High();
		}
		else
		{
			SDA_Pin_Output_Low();
		}	
			
        SCL_Pin_Output_High();
		Delay_3us();
		
		Data <<= 1;
		 
	}
	SCL_Pin_Output_Low();
	SensorDelay_us(8);   
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
}	


uint8_t ZSSC_I2C_RD_Byte(void)//��AHT10��ȡһ���ֽ�
{
	uint8_t Byte,i,a;
	Byte = 0;
	SCL_Pin_Output_Low();
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	for(i=0;i<8;i++)
	{
    SCL_Pin_Output_High();		
		Delay_1us();
		a=0;
		if(Gpio_GetIO(1,5))a=1;
		Byte = (Byte<<1)|a;
		SCL_Pin_Output_Low();
		Delay_1us();
	}
    SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	return Byte;
}


uint8_t Receive_ACK(void)   //��AHT10�Ƿ��лظ�ACK
{
	uint16_t CNT;
	CNT = 0;
	SCL_Pin_Output_Low();	
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	SCL_Pin_Output_High();	
	SensorDelay_us(8);	
	while((Gpio_GetIO(1,5))  && CNT < 100) 
	CNT++;
	if(CNT == 100)
	{
		return 0;
	}
 	SCL_Pin_Output_Low();	
	SensorDelay_us(8);	
	return 1;
}

void Send_ACK(void)		  //�����ظ�ACK�ź�
{
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);	
	SDA_Pin_Output_Low();
	SensorDelay_us(8);	
	SCL_Pin_Output_High();	
	SensorDelay_us(8);
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
}

void Send_NOT_ACK(void)	//�������ظ�ACK
{
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_High();	
	SensorDelay_us(8);		
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
    SDA_Pin_Output_Low();
	SensorDelay_us(8);
}

void Stop_I2C(void)	  //һ��Э�����
{
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();	
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
}

uint8_t JH_Read_Status(void)//��ȡAHT10��״̬�Ĵ���
{

	uint8_t Byte_first;	
	I2C_Start();
	ZSSC_I2C_WR_Byte(0x71);
	Receive_ACK();
	Byte_first = ZSSC_I2C_RD_Byte();
		

	Send_NOT_ACK();
	Stop_I2C();


	return Byte_first;
}

uint8_t JH_Read_Cal_Enable(void)  //��ѯcal enableλ��û��ʹ�ܣ�
{
	uint8_t val = 0;//ret = 0,
 
  val = JH_Read_Status();
  if((val & 0x68)==0x08)  //�ж�NORģʽ��У׼����Ƿ���Ч
  return 1;
  else  return 0;
 }



void JH_SendAC(void) //��AHT10����AC����
{

 I2C_Start();
 ZSSC_I2C_WR_Byte(0x70);
 Receive_ACK();
 ZSSC_I2C_WR_Byte(0xac);
 Receive_ACK();
 ZSSC_I2C_WR_Byte(0x33);
 Receive_ACK();
 ZSSC_I2C_WR_Byte(0x00);
 Receive_ACK();
 Stop_I2C();

}

void JH_Send_BA(void)//��AHT10����BA����
{


 I2C_Start();
 ZSSC_I2C_WR_Byte(0x70);
 Receive_ACK();
 ZSSC_I2C_WR_Byte(0xba);
 Receive_ACK();
 Stop_I2C();


}

void JH_Read_CTdata(uint32_t *ct) //��ȡAHT10���¶Ⱥ�ʪ������
{
	volatile uint8_t  Byte_1th=0;
	volatile uint8_t  Byte_2th=0;
	volatile uint8_t  Byte_3th=0;
	volatile uint8_t  Byte_4th=0;
	volatile uint8_t  Byte_5th=0;
	volatile uint8_t  Byte_6th=0;
	 uint32_t RetuData = 0;
	
	uint16_t cnt = 0;


	JH_SendAC();//��AHT10����AC����

	delay_ms(75);//�ȴ�75ms
    cnt = 0;
	while(((JH_Read_Status()&0x80)==0x80))//�ȴ�æ״̬����
	{
	 SensorDelay_us(1508);
	 if(cnt++>=100)
	 {
	  break;
	  }
	}
	I2C_Start();

	ZSSC_I2C_WR_Byte(0x71);//0x70+1   0x70Ϊ�豸��ַ 1Ϊ����λ
	Receive_ACK();
	Byte_1th = ZSSC_I2C_RD_Byte();//״̬��
	Send_ACK();
	Byte_2th = ZSSC_I2C_RD_Byte();//ʪ���ֽ�
	Send_ACK();
	Byte_3th = ZSSC_I2C_RD_Byte();//ʪ���ֽ�
	Send_ACK();
	Byte_4th = ZSSC_I2C_RD_Byte();//��4λΪʪ��  ��4λΪ�¶�
	Send_ACK();
	Byte_5th = ZSSC_I2C_RD_Byte();//�¶��ֽ�
	Send_ACK();
	Byte_6th = ZSSC_I2C_RD_Byte();//�¶��ֽ�
	Send_NOT_ACK();
	Stop_I2C();

	RetuData = (RetuData|Byte_2th)<<8;
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	RetuData =RetuData >>4;
	ct[0] = RetuData;
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	RetuData = RetuData&0xfffff;
	ct[1] =RetuData; 

}


u8 JH_Init(void)   //��ʼ��AHT10
{

	
	Init_I2C_Sensor_Port();
	SensorDelay_us(11038);

	I2C_Start();
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0xe1);//дϵͳ���üĴ���
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x08);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

	delay_ms(500);//��ʱ0.5S
   while(JH_Read_Cal_Enable()==0)//��Ҫ�ȴ�״̬��status��Bit[3]=1ʱ��ȥ�����ݡ����Bit[3]������1 ���������λ0xBA��AHT10�������³�ʼ��AHT10��ֱ��Bit[3]=1
   {
    
	JH_Send_BA();  //��λ
	delay_ms(100);
	SensorDelay_us(11038);

	I2C_Start();
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0xe1);//дϵͳ���üĴ���
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x08);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();
	count++;
	if(count>=10)return 0;
	delay_ms(500);
    }
   return 1;
}

int32_t main(void)
{
 uint32_t CT_data[2];
	volatile int  c1,t1;
	u8 ret=0;
	ret = JH_Init(); //��ʼ��
	if(ret == 0)
	{
	 LCD_display("ERROR"):
	 while(1);
	 }
	
	while(1)
	{
	 while(JH_Read_Cal_Enable()==0)//�ȵ�У׼���ʹ��λΪ1���Ŷ�ȡ��
	 {
	  JH_Init();//���Ϊ0��ʹ��һ��
      delay_ms(30);
	 }
	 //DisableIrq(); //������ģ��I2C,Ϊ��ȡ���ݸ�׼ȷ ����ȡ֮ǰ��ֹ�ж�
	 JH_Read_CTdata(CT_data);  //��ȡ�¶Ⱥ�ʪ�� �� �ɼ��1.5S��һ��
	 c1 = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵ���Ŵ���10��,���c1=523����ʾ����ʪ��Ϊ52.3%��
	 t1 = CT_data[1] *200*10/1024/1024-500;//����õ��¶�ֵ���Ŵ���10�������t1=245����ʾ�����¶�Ϊ24.5�棩
	 //EnableIrq(); //�ָ��ж�
	 delay_ms(1500); //��ʱ1.5S
	 
	 //Ϊ��ȡ�����ݸ��ȶ���������ʹ��ƽ��ֵ�˲����ߴ����˲�������ǰ���ȡ��ֵ������ֵ����̫��
	 }
	

 }	
