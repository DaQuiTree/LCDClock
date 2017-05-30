#define _TIME_C
#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "lunar.h"
#include "DHT11.h"
#include "keyboard.h"
#include "main.h"

uint8 pSec = 0xAA;
uint8 pDay = 0xAA;
uint8 pHour = 0xAA;

uint16 code weekCN[8][2] = {
	{0x0000,0x0000},{0xD6DC,0xD2BB},
	{0xD6DC,0xB6FE},{0xD6DC,0xC8FD},
	{0xD6DC,0xCBC4},{0xD6DC,0xCEE5},
	{0xD6DC,0xC1F9},{0xD6DC,0xC8D5}
};	/*空、周一~周日*/

void ClearInforArea()  //清空屏幕下三行,保留标题栏
{
	uint8 i;

	for(i = 0; i < 3; i++)
	{
		LCDClearArea(0,i+1);
	}
}

void ShowCurrentTime()
{
	uint16 pdata tStr[8];
	uint8  pdata dStr[16];
	uint16 pdata dSplit;

	DS1302BurstRead(&timeMod); 
	if(pSec != timeMod.sec)
	{
		pSec = timeMod.sec;
		tStr[0] = (timeMod.hour >> 4) + 0xA3B0;	//时高
		tStr[1] = (timeMod.hour & 0x0F) + 0xA3B0;//时低
		tStr[2] = 0xA1C3;//时间分隔符
		tStr[3] = (timeMod.min >> 4) + 0xA3B0;	//分高
		tStr[4] = (timeMod.min & 0x0F) + 0xA3B0;//分低
		tStr[5] = 0xA1C3;//时间分隔符
		tStr[6] = (timeMod.sec >> 4) + 0xA3B0;	//秒高
		tStr[7] = (timeMod.sec & 0x0F) + 0xA3B0;//秒低 
		LCDShowCN(0,3,tStr,8);//液晶第1行显示日期时间
		if(pHour != timeMod.hour){
			pHour = timeMod.hour;
			if(timeMod.hour >= 0x08 && timeMod.hour <= 0x18)	//若在白天，显示太阳图标
			{
				LCDDrawArea(5,0,sunImage);
			}else{	  //若在黑夜，显示月亮图标
				LCDDrawArea(5,0,sunImage);
			}
		}
	}

	if(pDay != timeMod.day)	//若日期改变了
	{
		pDay = timeMod.day;	  //更新日期
		dStr[0] = ' ';
		dStr[1] = ' ';
		dStr[2] = '2';
		dStr[3] = '0';
		dStr[4] = (timeMod.year >> 4) + '0';	//年高
		dStr[5] = (timeMod.year & 0x0F) + '0';//年低
		dStr[6] = ' ';
		dStr[7] = ' ';
		dStr[8] = (timeMod.month >> 4) + '0';	//月高
		dStr[9] = (timeMod.month & 0x0F) + '0';//月低
		dStr[10] = ' ';
		dStr[11] = ' ';
		dStr[12] = (timeMod.day >> 4) + '0';	//日高
		dStr[13] = (timeMod.day & 0x0F) + '0';//日低
		dSplit = 0xA1A4;

		LCDShowStr(0,1,dStr,14);//液晶第2行显示日期时间
		LCDShowCN(3,1,&dSplit,1);
		LCDShowCN(5,1,&dSplit,1);	//显示日期分隔符
		LCDShowCN(3,2,weekCN[timeMod.week],2);	//液晶第3行显示星期

	}  
}

void ShowTemp()
{
	uint8 temp,humi;
	uint16 dat[3];

	DHT11GetData(&humi, &temp);
	
	dat[0] = temp/10 + 0xA3B0;
	dat[1] = temp%10 + 0xA3B0;
	dat[2] = 0xA1E6; 
	LCDShowCN(3,1,dat,3);  //温度

	dat[0] = humi/10 + 0xA3B0;
	dat[1] = humi%10 + 0xA3B0;
	dat[2] = 0xA3A5; 
	LCDShowCN(3,2,dat,3);  //湿度
}

void ShowInfo()
{
	uint16 pdata lunar[7];
	uint8 pdata lunarLen, lday, lyear, lmonth;
	uint16 code iPrompt[3][3] ={
		{0xCEC2,0xB6C8,0xA3BA},
		{0xCAAA,0xB6C8,0xA3BA},
		{0xC4D6,0xD6D3,0xA3BA}
	};
	
	LCDClearAll();	  //清除显示区域
	LCDShowCN(0,1,iPrompt[0],3);  //p温度：
	LCDShowCN(0,2,iPrompt[1],3);  //p湿度：
	LCDShowCN(0,3,iPrompt[2],3);  //p闹钟：
	ShowTemp();				   //显示数值
	lyear = (timeMod.year >> 4) * 10 + (timeMod.year & 0x0F);
	lmonth = (timeMod.month >> 4) * 10 + (timeMod.month & 0x0F);
	lday = (timeMod.day >> 4) * 10 + (timeMod.day & 0x0F); 
	lunarLen = GetLunarDate(lyear, lmonth, lday, lunar);
	LCDShowCN(1,0,lunar,lunarLen);
}

void KeyAction(uint8 keyCode)
{
	if(keyCode == 0x0D)//回车
	{
		mMode++;
		mMode &= 0x01;//改变mMode状态
		if(mMode == ReadDate){
			LCDClearAll();	//清屏
		   	pSec = 0xAA;	//强制显示时间
			pDay = 0xAA;
			pHour = 0xAA;
		}else if(mMode == ReadInfo){
			ShowInfo();
		}
	}else if(keyCode == 0x1B){//取消

	}else if(keyCode == 0x26){//向上

	}else if(keyCode == 0xD0){
			LCDClearAll();	//清屏
			mMode 	
	}
}