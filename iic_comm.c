#include "iic_comm.h"
#include "Fx2.h"
#include "fx2regs.h"
#include <intrins.h>
 
void I2CInit()
{
	SDA = 1;
	SCL = 1;
}
 
void I2CStart()
{
	SDA = 0;
	_nop_();
	SCL = 0;
}
 
void I2CRestart()
{			 

	SDA = 1;
	SCL = 1;
	SDA = 0;
	SCL = 0;
}
 
void I2CStop()
{
	SCL = 0;
	SDA = 0;
	SCL = 1;
	SDA = 1;
}
 
void I2CAck()
{
	SDA = 0;
	SCL = 1;
	SCL = 0;
	SDA = 1;
}
 
void I2CNak()
{
	SDA = 1;
	SCL = 1;
	SCL = 0;
	SDA = 1;
}

unsigned char I2CSend(unsigned char Data)
{
	 unsigned char i, ack_bit;
	 for (i = 0; i < 8; i++) {
		if ((Data & 0x80) == 0)
			SDA = 0;
		else
			SDA = 1;
		SCL = 1;
		_nop_();
		_nop_();
		_nop_();
	 	SCL = 0;
		Data<<=1;
	 }
	 SDA = 1;
	 SCL = 1;
	 _nop_();
	 ack_bit =SDA;//;//(unsigned char) SDA;
	 SCL = 0;
	 //OEB=0x03;
	 return ack_bit;
}
unsigned char I2CRead()
{
	unsigned char i, Data=0;
	for (i = 0; i < 8; i++) {
		SCL = 1;
		if(SDA)
			Data |=1;
		if(i<7)
			Data<<=1;
		SCL = 0;
	}
	return Data;
}
