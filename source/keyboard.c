#define _KEYBOARD_C
#include "config.h"
#include "keyboard.h"
#include "time.h"

bit flag200ms = 0;
bit flag1s = 0;
bit flag5s = 0;

uint8 T0RH = 0;
uint8 T0RL = 0;

uint8 pdata keySta[4][4]={
	{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}
};

uint8 code keyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
    { 0x31, 0x32, 0x33, 0x26 }, //数字键1、数字键2、数字键3、向上键
    { 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
    { 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
    { 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
};

void ClearTremble()
{
	uint8 i;
	static uint8 queue = 0; 

	static uint8 pdata state[4][4] = 
	{
		{0x0F,0x0F,0x0F,0x0F},
		{0x0F,0x0F,0x0F,0x0F},
		{0x0F,0x0F,0x0F,0x0F},
		{0x0F,0x0F,0x0F,0x0F}
	};

	switch(queue)
	{
		case 0:KeyOut4 = 1;KeyOut1 = 0;break;
		case 1:KeyOut1 = 1;KeyOut2 = 0;break;
		case 2:KeyOut2 = 1;KeyOut3 = 0;break;
		case 3:KeyOut3 = 1;KeyOut4 = 0;break;
		default:break;
	}

	state[queue][0] = (state[queue][0] << 1) | KeyIn1;
	state[queue][1] = (state[queue][1] << 1) | KeyIn2;
	state[queue][2] = (state[queue][2] << 1) | KeyIn3;
	state[queue][3] = (state[queue][3] << 1) | KeyIn4;

	for(i = 0; i < 4; i++)
	{
		if((state[queue][i] & 0x0F) == 0x0F)
		{
			keySta[queue][i] = 1;
		}else if((state[queue][i] & 0x0F) == 0x00){
			keySta[queue][i] = 0;
		}else{
		}
	}

	queue++;
	queue &= 0x03;
}

void KeyDriver()
{
	static uint8 pdata keyBuf[4][4]={
		{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}	
	};
	static uint16 ethBuf = 0, enterThreshold = 1000;

	uint8 i,j;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(keyCodeMap[i][j] == 0x26) //长按向上键连续增加
			{
				if(keyBuf[i][j] == 0){
					ethBuf += 1;
					if(ethBuf > enterThreshold){
						KeyAction(0xD0);
						enterThreshold += 200;
					}
				}else{
					ethBuf = 0;
				}		
			}

			if(keyBuf[i][j] != keySta[i][j])
			{
				if(keyBuf[i][j] == 0x01)
				{
					KeyAction(keyCodeMap[i][j]);
				}
				keyBuf[i][j] = keySta[i][j];
			}
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
}

void InterruptTimerZero() interrupt 1
{
	static uint8 cnt200ms = 0, cnt1s = 0, cnt5s = 0;

	TH0 = T0RH;
	TL0 = T0RL;

	cnt200ms++;			
	if(cnt200ms >= 200) //200ms定时器
	{
		cnt200ms = 0;
		flag200ms = 1;
		cnt1s++;		//1s定时器
		if(cnt1s >= 5)
		{
			cnt1s = 0;
			flag1s = 1;
		}
		cnt5s++;
		if(cnt5s >= 25)
		{
			cnt5s = 0;
			flag5s = 1;
		}
	}
	ClearTremble();
}
