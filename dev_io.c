#include "Fx2.h"
#include "fx2regs.h"
#include "LED_Key.h"

#include "iic_comm.h"
#include "ssd1306.h"

#include "esp8266.h"
#include "MQTTPACKET.H"	
#include "string.h"

code char Subs[96]=
{0x82,0x5e,0x00,0x01,0x00,0x59,0x76,0x31,0x2f,0x37,0x37,0x32,0x36,0x31,0x62,0x62
,0x30,0x2d,0x34,0x64,0x32,0x65,0x2d,0x31,0x31,0x65,0x37,0x2d,0x61,0x31,0x63,0x38
,0x2d,0x30,0x33,0x61,0x33,0x31,0x30,0x62,0x30,0x37,0x62,0x62,0x32,0x2f,0x74,0x68
,0x69,0x6e,0x67,0x73,0x2f,0x64,0x63,0x37,0x37,0x63,0x32,0x32,0x30,0x2d,0x34,0x64
,0x38,0x33,0x2d,0x31,0x31,0x65,0x37,0x2d,0x39,0x30,0x64,0x63,0x2d,0x63,0x64,0x36
,0x33,0x39,0x63,0x61,0x39,0x30,0x39,0x61,0x38,0x2f,0x63,0x6d,0x64,0x2f,0x32,0x00};

code char Publish[93]=
{0x30,0x5d,0x00,0x5a,0x76,0x31,0x2f,0x37,0x37,0x32,0x36,0x31,0x62,0x62,0x30,0x2d
,0x34,0x64,0x32,0x65,0x2d,0x31,0x31,0x65,0x37,0x2d,0x61,0x31,0x63,0x38,0x2d,0x30
,0x33,0x61,0x33,0x31,0x30,0x62,0x30,0x37,0x62,0x62,0x32,0x2f,0x74,0x68,0x69,0x6e
,0x67,0x73,0x2f,0x64,0x63,0x37,0x37,0x63,0x32,0x32,0x30,0x2d,0x34,0x64,0x38,0x33
,0x2d,0x31,0x31,0x65,0x37,0x2d,0x39,0x30,0x64,0x63,0x2d,0x63,0x64,0x36,0x33,0x39
,0x63,0x61,0x39,0x30,0x39,0x61,0x38,0x2f,0x64,0x61,0x74,0x61,0x2f};


 
extern unsigned char temp_dataT[70];
extern unsigned char count;
unsigned char Lig=1;
unsigned char CountineCheck=0;



void MQTTPublish0(char channel, char value)
{
	unsigned char len=0;
	for(len=0;len<93;len++)
	{
		Serial0_SendChar(Publish[len]);
	}
	Serial0_SendChar(channel+0x30);
	Serial0_SendChar(value+0x30);
}


code char clientID[]="dc77c220-4d83-11e7-90dc-cd639ca909a8";
code char username[]="77261bb0-4d2e-11e7-a1c8-03a310b07bb2";
code char password[]="6ecdc74199d4b43d7c01c4c640162b71ab50a77c";
void MQTTConnect()
{
	int len=0;
	Serial0_SendChar(0x10);//header
	len=12;
	len+=(strlen(clientID)+2);
	len+=(strlen(username)+2);
	len+=(strlen(password)+2);
	do{
		unsigned char SendByte=len%128;
		len=len/128;
		if(len>0)
		{
			SendByte|=0x80;
		}
		Serial0_SendChar(SendByte);
	}while (len>0);  //length

	Serial0_SendInt(6);//length of MQIsqd
	Serial0_SendString("MQIsdp");
	Serial0_SendChar(3);//version of MQTT
	Serial0_SendChar(0xc2);//flags
	Serial0_SendInt(60);//time interval
	
	Serial0_SendInt(strlen(clientID));
	Serial0_SendString(clientID);
	Serial0_SendInt(strlen(username));
	Serial0_SendString(username);
	Serial0_SendInt(strlen(password));
	Serial0_SendString(password);	
}


void MQTTPublish1(char channel, char value)
{
	int len=0;
	Serial0_SendChar(0x32);//header
	len=23;
	len+=(strlen(clientID));
	len+=(strlen(username));
	do{
		unsigned char SendByte=len%128;
		len=len/128;
		if(len>0)
		{
			SendByte|=0x80;
		}
		Serial0_SendChar(SendByte);
	}while (len>0);  //length

	Serial0_SendChar(0);
	Serial0_SendChar(0X5A);
	Serial0_SendString("v1/");
	Serial0_SendString(username);
	Serial0_SendString("/things/");
	Serial0_SendString(clientID);
	Serial0_SendString("/data/");
	Serial0_SendChar(channel+0x30);
	Serial0_SendChar(0x00);
	Serial0_SendChar(0x01);
	Serial0_SendChar(value+0x30);	
}

void main(void)
{ 
	int Light1=0;
	int i;

	int len=0;
	MQTTHeader header = {0};
	MQTTConnectFlags flags = {0};

	OED=0xff;
	OEB=0xff;
	OEE=0x0f;
	Delay(1000);
	PD5=1;
	LEDFlash();
	
	I2CInit();

	ssd1306_initalize();
	ssd1306_clear();
	ssd1306_printf("|   Mydevices   |   IOT Model   |   By ZY Xiao  |     2017-7    ");
		
	//ssd1306_draw();
	
		   
	Serial0_Init();

Restart:
	Serial0_SendString("+++");
	Delay(5000);
	
	Serial0_SendString("AT+RST");
	Serial0_SendChar(0x0d);
	Serial0_SendChar(0x0a);
	Serial0_SendChar(0x00);
	Delay(10000);
	ABCShow();

	Delay(15000);
	ABCShow();

	Serial0_SendString("AT+CIPSTART=\"TCP\",\"mqtt.mydevices.com\",1883");
	Serial0_SendChar(0x0d);
	Serial0_SendChar(0x0a);
	Serial0_SendChar(0x00);
	Delay(10000);
	ABCShow();
	
	Serial0_SendString("AT+CIPMODE=1");
	Serial0_SendChar(0x0d);
	Serial0_SendChar(0x0a);
	Serial0_SendChar(0x00);
	Delay(10000);
	ABCShow(); 
	
	Serial0_SendString("AT+CIPSEND");
	Serial0_SendChar(0x0d);
	Serial0_SendChar(0x0a);
	Serial0_SendChar(0x00);
	Delay(10000); 
	ABCShow();
						
	MQTTConnect();
	Delay(10000); 

	if (count!=4) goto Restart;

	ABC1Show();
	Delay(3000);

	//subscribe
	for(len=0;len<96;len++)
	{
		Serial0_SendChar(Subs[len]);
	}
	Delay(10000); 
	ABC1Show();
	Delay(3000);

	PD7=0;
	Light1=0;
	MQTTPublish0(2,0);
	MQTTPublish0(3,0);

	ssd1306_initalize();
	ssd1306_clear();
	if (Light1==1){
		ssd1306_printf("Channel 1: ON");
	}
	else
	{
		ssd1306_printf("Channel 1: OFF");
	}

	while(1)
	{	
		//temp_dataT[0]=count;ABC1Show();Delay(10000);
		
		//39=0x2b 
		if (count==0x27)
		{			
			//temp_dataT[0]=temp_dataT[count-1];	
			if (temp_dataT[count-1]==0x30)
			{
				PD7=0;
				Light1=0;
				MQTTPublish0(2,0);
				MQTTPublish0(3,0);
	
				ssd1306_printf("Channel 1: OFF");
			}
			else
			{
				PD7=1;
				Light1=1;
				MQTTPublish0(2,1);
				MQTTPublish0(3,1);
	
				ssd1306_printf("Channel 1: ON");
			}
			count=0;
		}
	
		count=0;
		MQTTPublish1(1, 0);//will receive
		Delay(500);
		if (count==4)
		{	
			CountineCheck=0; 
			count=0;
		}
		else
		{
			CountineCheck++; 
			count=0;
		}
		if (CountineCheck==10)	//disconnect check;
			goto Restart;

		if (Light1==1)
		{	
			MQTTPublish0(3,1);	
		}
		else
		{
			MQTTPublish0(3,0);	
		}
		
		//Delay(500);
		MQTTPublish0(1, 1);
		Delay(500);
		count=0;
	
		//check key
		for(i=0;i<300;i++)
		{
			 if (ScanKey())
			 {
				if (Light1==0){
					Light1=1;
					PD7=1;
				    MQTTPublish0(2,1);
					MQTTPublish0(3,1);
					ssd1306_printf("Channel 1: ON");
				}
				else
				{
					Light1=0;
					PD7=0;
					MQTTPublish0(2,0);
					MQTTPublish0(3,0);
					ssd1306_printf("Channel 1: OFF");
				}
				Delay(500);
				count=0;
			 }	
			 Delay(10);	 
		}
		//Delay(1000);
		//end of the while loop
	}
	//connect internet
	/*Serial0_SendString("AT+CWJAP=\"xxxx\",\"xxxx\"");
	Serial0_SendChar(0x0d);
	Serial0_SendChar(0x0a);
	Serial0_SendChar(0x00);*/
}
