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
uint8 pHour = 0xAA; //时钟的初始值

uint8 cHour = 0x00;
uint8 cMin = 0x00; //闹钟初始值

sTime timeBuf; //存储着十进制时间

uint8 curPos = 0;

uint8 code DateMap[12][2] = {
	{1,31},{2,28},{3,31},{4,30},{5,31},{6,30},
	{7,31},{8,31},{9,30},{10,31},{11,30},{12,31}
};

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

void RightShiftDate()
{
	switch(curPos)
	{
		case 0: LCDSetCursor(4,1);break;
		case 1: LCDSetCursor(6,1);break;
		case 2: LCDSetCursor(4,2);break;
		case 3: LCDSetCursor(1,3);break;
		case 4: LCDSetCursor(4,3);break;
		case 5: LCDSetCursor(7,3);break;
		case 6: LCDSetCursor(2,1);break;
		default: break;
	}
	curPos++;
	if(curPos > 5) curPos = 0;
}

void RightShiftClock()
{
	switch(curPos)
	{
		case 0: LCDSetCursor(6,2);break;
		case 1: LCDSetCursor(3,2);break;
		default: break;
	}
	curPos++;
	curPos &= 0x01;	
}

void ShowAdjusted()				  
{
	uint8 pdata strTmp[2];
	uint16 pdata CNTmp[2];

	if(mMode == SetDate)
	{
		case 0:
			strTmp[0] = (timeBuf.year >> 4) + '0';
			strTmp[1] = (timeBuf.year & 0x0F) + '0';
			LCDShowStr(2,1,strTmp,2);
			break;
		case 1:
			strTmp[0] = (timeBuf.month >> 4) + '0';
			strTmp[1] = (timeBuf.month & 0x0F) + '0';
			LCDShowStr(4,1,strTmp,2);
			break;
		case 2:
			strTmp[0] = (timeBuf.day >> 4) + '0';
			strTmp[1] = (timeBuf.day & 0x0F) + '0';
			LCDShowStr(6,1,strTmp,2);
			break;
		case 3:
			LCDShowCN(3,2,weekCN[timeBuf.week],2);
			break;			
		case 4:
			CNTmp[0] = (timeBuf.hour >> 4) + 0xA3B0;
			CNTmp[1] = (timeBuf.hour & 0x0F) + 0xA3B0;
			LCDShowStr(0,3,CNTmp,2);
			break;
		case 5:
			CNTmp[0] = (timeBuf.min >> 4) + 0xA3B0;
			CNTmp[1] = (timeBuf.min & 0x0F) + 0xA3B0;
			LCDShowStr(3,3,CNTmp,2);
			break;
		case 6:
			CNTmp[0] = (timeBuf.sec >> 4) + 0xA3B0;
			CNTmp[1] = (timeBuf.sec & 0x0F) + 0xA3B0;
			LCDShowStr(6,3,CNTmp,2);
			break;
		default: break;
	}else if(mMode == SetClock){
		case 0:
			CNTmp[0] = (cHour >> 4) + 0xA3B0;
			CNTmp[1] = (cHour & 0x0F) + 0xA3B0;
			LCDShowStr(2,2,CNTmp,2);
			break;
		case 1:
			CNTmp[0] = (cMin >> 4) + 0xA3B0;
			CNTmp[1] = (cMin & 0x0F) + 0xA3B0;
			LCDShowStr(5,2,CNTmp,2);
			break;
		default: break;
	}		
}

void AdjustDate()
{
	switch(curPos)
	{
		case 0:			//调整年
			timeBuf.year++;
			if(timeBuf.year > 99) timeBuf.year = 0;
			break;
		case 1:			 //调整月
			timeBuf.month++;
			if(timeBuf.month > 12) timeBuf.month = 1;
			break;
		case 2:			//日
			timeBuf.day++;
			if(timeBuf.year % 4 == 0){	//闰年二月单独处理
				if(timeBuf.month == 2){
					if(timeBuf.day > 29) timeBuf.day = 1;
					break;
				}; 
			}
			if(timeBuf.day > DateMap[timeBuf.month-1][1]) timeBuf.day = 1;
			break;
		case 3:		   //周
			timeBuf.week++;
			if(timeBuf.week > 7) timeBuf.week = 1;
			break;
		case 4:		   //时
			timeBuf.hour++;
			if(timeBuf.hour > 23) timeBuf.hour = 0;
			break;
		case 5:		   //分
			timeBuf.min++;
			if(timeBuf.min > 59) timeBuf.min = 0;
			break;
		case 6:		   //秒
			timeBuf.sec++;
			if(timeBuf.sec > 59) timeBuf.sec = 0;
			break;
		default: break;
	}
	ShowAdjusted();
}

void AdjustClock()
{
	switch(curPos)
	{
		case 0:			//调整Clock时
			cHour++;
			if(cHour > 23) cHour = 0;
			break;
		case 1:			 //调整Clock分
			cMin++;
			if(cMin > 59) cMin = 0;
			break;
		default: break;
	}
	ShowAdjusted();
}

void GetDecimalTime()
{
	timeBuf.year = (timeMod.year >> 4)*10 + (timeMode.year & 0x0F);
	timeBuf.month = (timeMod.month >> 4)*10 + (timeMode.month & 0x0F);
	timeBuf.day = (timeMod.day >> 4)*10 + (timeMode.day & 0x0F);
	timeBuf.week =  timeMod.week;
	timeBuf.hour = (timeMod.hour >> 4)*10 + (timeMode.hour & 0x0F);
	timeBuf.min = (timeMod.min >> 4)*10 + (timeMode.min & 0x0F);
	timeBuf.sec = (timeMod.sec >> 4)*10 + (timeMode.sec & 0x0F);
}

void KeyAction(uint8 keyCode)
{
	if(keyCode == 0x0D)//按回车键进入设定状态
	{
		uint16 pdata clockStr[5];

		if(mMode == SetDate){
			mMode = SetClock;
			LCDClearAll();
			LCDShowCN(0,0,"闹钟设定：",5);
			clockStr[0] = (cHour >> 4) + 0xA3B0;
			clockStr[1] = (cHour & 0x0F) + 0xA3B0;
			clockStr[2] = 0xA1C3;
			clockStr[3] = (cMin >> 4) + 0xA3B0;
			clockStr[4] = (cMin & 0x0F) + 0xA3B0;
			LCDShowCN(2,2,clockStr,5);
			curPos = 0;
			LCDSetCursor(3,2);
			LCDShowCursor();
		}else{
			mMode = SetDate;
			LCDClearAll();
			clockStr[0] = 0xD0A3;
			clockStr[1]	= 0xCAB1;
			LCDShowCN(2,0,clockStr,2);//为节省内存借clockStr一用
			LCDDrawArea(5,0,SaveButton);
			pSec = 0xAA;
			pDay = 0xAA; //强制刷新时间界面（不刷新太阳月亮图标）
			ShowCurrentTime();
			GetDecimalTime();//将BCD时间转换为十进制时间
			curPos = 0;
			LCDSetCursor(2,1);
			LCDShowCursor();
			
		}
	}else if(keyCode == 0x1B){//取消

	}else if(keyCode == 0x26){//向上
		if(mMode == SetDate){
			AdjustDate();
		}else if(mMode == SetClock){
			AdjustClock();
		}		
	}else if(keyCode == 0x27){//向右、翻页
		if(mMode < SetDate) //处于信息展示状态时翻页
		{
			mMode++;
			mMode &= 0x01;//改变mMode状态
			if(mMode == ReadDate){ //时间界面
				LCDClearAll();	//清屏
			   	pSec = 0xAA;	//强制显示时间
				pDay = 0xAA;
				pHour = 0xAA;
			}else if(mMode == ReadInfo){//温度等信息界面
				ShowInfo();
			}
		}else{	 //处于设定状态右键相当于调整设定位置
			if(mMode == SetDate){
				RightShiftDate();
			}else if(mMode == SetClock){
				RightShiftClock();
			}
		}		
	}
}