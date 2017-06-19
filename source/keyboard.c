#define _KEYBOARD_C
#include "config.h"
#include "keyboard.h"
#include "time.h"

bit flag200ms = 0;
bit flag1s = 0;
bit flagBuzzOn = 0;

uint8 T0RH = 0;
uint8 T0RL = 0;

uint8 pdata keySta[4]={1,1,1,1};

uint8 code keyCodeMap[4] = { //矩阵按键编号到标准键盘键码的映射表
	0x26, 0x1B, 0x0D, 0x27   //向上键、 ESC键、  回车键、 向右键
};

void ClearTremble()
{
	uint8 i;

	static uint8 pdata state[4] = {0x0F,0x0F,0x0F,0x0F};

	state[0] = (state[0] << 1) | KeyAdj;
	state[1] = (state[1] << 1) | KeyCancel;
	state[2] = (state[2] << 1) | KeySet;
	state[3] = (state[3] << 1) | KeyPage;

	for(i = 0; i < 4; i++)
	{
		if((state[i] & 0x0F) == 0x0F)
		{
			keySta[i] = 1;
		}else if((state[i] & 0x0F) == 0x00){
			keySta[i] = 0;
		}else{
		}
	}
}

void KeyDriver()
{
	static uint8 pdata keyBuf[4]={1,1,1,1};
	static uint16 ethBuf = 0, enterThreshold = 1000;

	uint8 i;

	for(i = 0; i < 4; i++)
	{
		if(keyCodeMap[i] == 0x26) //长按向上键连续增加
		{
			if(keyBuf[i] == 0){
				ethBuf += 1;
				if(ethBuf > enterThreshold){
					KeyAction(0x26);
					enterThreshold += 100;
				}
			}else{
				ethBuf = 0;
				enterThreshold = 1000;
			}		
		}

		if(keyBuf[i] != keySta[i])
		{
			if(keyBuf[i] == 0x01)
			{
				KeyAction(keyCodeMap[i]);
			}
			keyBuf[i] = keySta[i];
		}
	}	
}

void ConfigTimerZero(uint8 ms)
{
	uint16 tmp;

	tmp = 65536 - SYS_CLK/1000*ms + 18;

	T0RH = (uint8)(tmp >> 8);
	T0RL = (uint8)(tmp & 0x0F);

	TMOD &= 0xF0;
	TMOD |= 0x01;

	TH0 = T0RH;
	TL0 = T0RL;

	TR0 = 1;
	ET0 = 1;
	PT1 = 1;
}

void InterruptTimerZero() interrupt 1
{
	static uint8 cnt100ms = 0, cnt200ms = 0, cnt1s = 0, flute = 0;
	static uint16 cnt300ms = 0;

	TH0 = T0RH;
	TL0 = T0RL;

	cnt100ms++;			
	if(cnt100ms >= 100) //200ms定时器
	{
		cnt100ms = 0;
		cnt200ms++;
		if(cnt200ms >= 2)
		{
			cnt200ms = 0;
			flag200ms = 1;
		}
		cnt1s++;		//1s定时器
		if(cnt1s >= 10)
		{
			cnt1s = 0;
			flag1s = 1;
		}
	}

	if(flagBuzzOn)
	{
		switch(flute)
		{
			case 0:
				BUZZ = 0; //初始
			case 2:
			case 4:
				if(cnt100ms >= 99){
					flute++;
					BUZZ = 1;
				}
				break;
			case 1:
			case 3:
			case 5:
				if(cnt100ms >= 99){
					flute++;
					BUZZ = 0;
				}
				break;				
			case 6:
				cnt300ms++;
				if(cnt300ms >= 300)
				{
					cnt300ms = 0;
					flute++;
					BUZZ = 1;
				}
				break;
			case 7:
				if(cnt100ms >= 99)flute = 0;
				break;
			default: break;
		}
	}

	ClearTremble();
}
