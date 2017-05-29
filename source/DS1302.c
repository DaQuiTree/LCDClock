#define _DS1302_C
#include "config.h"
#include "DS1302.h"

struct sTime timeMod;

uint8 DS1302ReadByte()
{
	uint8 mask;
	uint8 dat = 0;

	for(mask = 0x01; mask != 0; mask <<= 1)
	{
		if(DS1302_IO){
			dat |= mask; 
		}
		DS1302_CK = 1;
		DS1302_CK = 0;	
	}
	return dat;
}

void DS1302WriteByte(uint8 dat)
{
	uint8 mask;

	for(mask = 0x01; mask != 0; mask <<= 1)
	{
		if((mask & dat) == 0){
			DS1302_IO = 0;
		}else{
			DS1302_IO = 1;
		}
		
		DS1302_CK = 1;
		DS1302_CK = 0;		
	}
}

void DS1302BurstRead(struct sTime* time)
{
	DS1302_CE = 1;
	DS1302WriteByte(0xBF); //Brust读模式
	time->sec = DS1302ReadByte();
	time->min = DS1302ReadByte();
	time->hour = DS1302ReadByte();
	time->day = DS1302ReadByte();
	time->month = DS1302ReadByte();
	time->week = DS1302ReadByte();
	time->year = DS1302ReadByte();
	DS1302_CE = 0;

	time->sec &= 0x7F;
}

void DS1302BurstWrite(struct sTime* time)
{
	DS1302_CE = 1;
	DS1302WriteByte(0xBE); //Brust写模式
	DS1302WriteByte(time->sec);
	DS1302WriteByte(time->min);
	DS1302WriteByte(time->hour);
	DS1302WriteByte(time->day);
	DS1302WriteByte(time->month);
	DS1302WriteByte(time->week);
	DS1302WriteByte(time->year);
	DS1302WriteByte(0x00);
	DS1302_CE = 0;	
}

uint8 DS1302SingleRead(uint8 reg)
{
	uint8 dat;

	DS1302_CE = 1;
	DS1302WriteByte((reg << 1) | 0x81);
	dat = DS1302ReadByte();
	DS1302_CE = 0;
	return dat;
}

void DS1302SingleWrite(uint8 reg, uint8 dat)
{	
	DS1302_CE = 1;
	DS1302WriteByte((reg << 1) | 0x80);
	DS1302WriteByte(dat);
	DS1302_CE = 0;
}

void InitDS1302()
{
	uint8 i;
	struct sTime initTime;

	initTime.year = 0x17;
	initTime.month = 0x05;
	initTime.day = 0x09;
	initTime.hour = 0x12;
	initTime.min = 0x55;
	initTime.sec = 0x00;
	initTime.week = 0x02;

	DS1302_CK = 0;
	DS1302_CE = 0;

	i = DS1302SingleRead(0);
	if((i & 0x80) != 0)	//如果1302没有处于工作状态
	{
		DS1302SingleWrite(7, 0x00); //取消写保护
		DS1302BurstWrite(&initTime); //进行初始化		
	}
}