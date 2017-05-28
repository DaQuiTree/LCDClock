#include "config.h"
#include "lcd12864.h"

#define _LCD12864_C

void LCDWaitReady()
{
	uint8 tmp = 0xFF;

	LCD12864_RS = 0;
	LCD12864_RW = 1;
	do{
		LCD12864_EN = 1;
		tmp = LCD12864_DB & 0x80;
		LCD12864_EN = 0;	
	}while(tmp != 0x00);
	LCD12864_EN = 0;	
}

void LCDWriteCmd(uint8 cmd)
{
	LCDWaitReady();
	LCD12864_RS = 0;
	LCD12864_RW = 0;
	LCD12864_EN = 1;
	LCD12864_DB = cmd;
	LCD12864_EN = 0;	
}

void LCDWriteData(uint8 dat)
{
	LCDWaitReady();
	LCD12864_RS = 1;
	LCD12864_RW = 0;
	LCD12864_EN = 1;
	LCD12864_DB = dat;
	LCD12864_EN = 0;
}

void InitLCD12864()
{
	LCDWriteCmd(0x30);
	LCDWriteCmd(0x0C);
	LCDWriteCmd(0x01); //清屏，(0,0)位置字符设定成空格
	LCDShowStr(0,0," ",1);
}

void LCDShowStr(uint8 x, uint8 y, uint8* str, uint8 strlen)	//写入字符串
{
	LCDSetCursor(x,y);
	do{
		LCDWriteData(*str++);
		strlen--;	
	}while(strlen > 0);
}

void LCDShowCN(uint8 x, uint8 y, uint16* str, uint8 strlen)	//写入字符串
{
	uint8 msb,lsb;

	while(strlen-- > 0){
		LCDSetCursor(x,y);
		x++;
		if(x > 7){
			x = 0;
			y++;
		} 			//先写入地址
		msb = (uint8)(*str >> 8);	//写入数据
		lsb = (uint8)(*str); 
		LCDWriteData(msb);
		LCDWriteData(lsb);
		str++;						
	}
}

void LCDSetCursor(uint8 x, uint8 y)
{
	uint8 pos = 0;

	switch(y)
	{
		case 0:
			pos = 0x80 + x;
			break;
		case 1:
			pos = 0x90 + x;
			break;
		case 2:
			pos = 0x88 + x;
			break;
		case 3:
			pos = 0x98 + x;
			break;
		default:
		 	break;
	}
	LCDWriteCmd(pos);
}

void LCDShowCursor()
{
	LCDWriteCmd(0x0F);
}

void LCDCancelCursor()
{
	LCDWriteCmd(0x0C);
}

void LCDClearAll()
{
	LCDWriteCmd(0x30);
	LCDWriteCmd(0x01);
	LCDShowStr(0,0," ",1);
}

void LCDShowImage(uint8* image)
{
	uint8 i,j;

	LCDWriteCmd(0x34); //扩充指令集设定关闭绘图显示
	LCDWriteCmd(0x34);
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //设置绘图垂直地址
		LCDWriteCmd(0x80);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(*image++);
			LCDWriteData(*image++);
		}
	}
	
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //设置绘图垂直地址
		LCDWriteCmd(0x88);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(*image++);
			LCDWriteData(*image++);
		}
	}	
	LCDWriteCmd(0x36);	
}
