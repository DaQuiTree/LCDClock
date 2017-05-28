#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "lunar.h"
#include "DHT11.h"

uint16* code weekCN[] = {
	"*","一","二","三","四","五","六","天"
};

void ShowCurrentTime()
{
	static uint8 pSec = 0xAA;
	static uint8 pDay = 0xAA;
	uint8  tStr[20];
	uint8  lunarLen, lday, lyear, lmonth;
	uint16 lunar[7];

	DS1302BurstRead(&timeMod); 
	if(pSec != timeMod.sec)
	{
		pSec = timeMod.sec;
		tStr[0] = ' ';
		tStr[1] = (timeMod.month >> 4) + '0';	//月高
		tStr[2] = (timeMod.month & 0x0F) + '0';//月低
		tStr[3] = '-';//日期分隔符
		tStr[4] = (timeMod.day >> 4) + '0';	//日高
		tStr[5] = (timeMod.day & 0x0F) + '0';//日低
		tStr[6] = ' '; 
		tStr[7] = (timeMod.hour >> 4) + '0';	//时高
		tStr[8] = (timeMod.hour & 0x0F) + '0';//时低
		tStr[9] = ':';//时间分隔符
		tStr[10] = (timeMod.min >> 4) + '0';	//分高
		tStr[11] = (timeMod.min & 0x0F) + '0';//分低
		tStr[12] = ':';//时间分隔符
		tStr[13] = (timeMod.sec >> 4) + '0';	//秒高
		tStr[14] = (timeMod.sec & 0x0F) + '0';//秒低
		LCDShowStr(0,1,tStr,15);//液晶第1行显示日期时间
	}
	if(pDay != timeMod.day)	//显示年、礼拜、农历
	{
		pDay = timeMod.day;
		tStr[0] = '2';	//公元2000年
		tStr[1] = '0';
		tStr[2] = (timeMod.year >> 4) + '0';	//年高
		tStr[3] = (timeMod.year & 0x0F) + '0';//年低
		LCDShowStr(1,0,tStr,4);//第0行显示年份
		LCDShowCN(4,0,"礼拜",2);
		LCDShowCN(6,0,weekCN[timeMod.week],1);
		lyear = (timeMod.year >> 4) * 10 + (timeMod.year & 0x0F);
		lmonth = (timeMod.month >> 4) * 10 + (timeMod.month & 0x0F);
		lday = (timeMod.day >> 4) * 10 + (timeMod.day & 0x0F); 
		lunarLen = GetLunarDate(lyear, lmonth, lday, lunar);
		LCDShowCN(1,2,lunar,lunarLen);
	}  
}

void ShowTemp()
{
	uint8 temp,humi;
	uint8 dat[16] = "  30    45%RH   ";
	uint16 symbol = 0xA1E6;

	DHT11GetData(&humi, &temp);
	dat[2] = temp/10 + '0';
	dat[3] = temp%10 + '0';
	dat[8] = humi/10 + '0';
	dat[9] = humi%10 + '0';

	LCDShowStr(0,3,dat,16);
	LCDShowCN(2,3,&symbol,1);
}