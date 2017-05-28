#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "keyboard.h"
#include "time.h"

bit flag200ms = 0;
uint8 T0RH = 0;
uint8 T0RL = 0;

void ConfigureTimerZero(uint8 ms);

void main()
{
	EA = 1;	
	InitDS1302();										  
	InitLCD12864();
	ConfigureTimerZero(1);

	while(1)
	{
		if(flag200ms){
			flag200ms = 0;
			ShowCurrentTime();
		}
	}
}

void ConfigureTimerZero(uint8 ms)
{
	uint16 tmp;

	tmp = 11059200/12/1000*ms;
	tmp = 65536 - tmp+18;

	T0RH = (uint8)(tmp >> 8);
	T0RL = (uint8) tmp;

	TMOD &= 0xF0;
	TMOD |= 0x01;

	TH0 = T0RH;
	TL0 = T0RL;

	TR0 = 1;
	ET0 = 1;
}

void InterruptTimerZero() interrupt 1
{
	static uint8 cnt = 0;

	TH0 = T0RH;
	TL0 = T0RL;	

	cnt++;
	if(cnt >= 200)
	{
		cnt = 0;
		flag200ms = 1;
	}
} 