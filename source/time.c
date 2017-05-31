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

struct sTime timeBuf; //存储着十进制时间

uint8 curPos = 0;
bit bClockOpen = 0;

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

	if(pHour == 0xAA) //显示小闹钟图标的时机和显示太阳/月亮时机相同
	{
		if(bClockOpen)
		{
			LCDDrawArea(1,0,clockImage);
		}else{
			LCDClearImage(1,0);
		}		
	}

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
				LCDDrawArea(5,0,moonImage);
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
	if(bClockOpen)	   //为节省内存借用lunar   
	{
		lunar[0] = cHour/10 + 0xA3B0; //显示闹钟具体时间
		lunar[1] = cHour%10 + 0xA3B0;
		lunar[2] = 0xA1C3;			
		lunar[3] = cMin/10 + 0xA3B0;
		lunar[4] = cMin%10 + 0xA3B0;
		LCDShowCN(3,3,lunar,5);
	}else{						
		lunar[0] = 0xCEB4;	// 显示“未启用”
		lunar[1] = 0xBFAA;		
		lunar[2] = 0xC6F4;		 
		LCDShowCN(3,3,lunar,3);	  
	}
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
		case 6: LCDSetCursor(5,0);break;
		case 7: LCDSetCursor(2,1);break;
		default: break;
	}
	curPos++;
	if(curPos > 7) curPos = 0;
}

void RightShiftClock()
{
	switch(curPos)
	{
		case 0: LCDSetCursor(6,2);break;
		case 1: LCDSetCursor(5,0);break;
		case 2: LCDSetCursor(3,2);break;
		default: break;
	}
	curPos++;
	if(curPos > 2) curPos = 0;	
}

void SaveTime()
{
	timeMod.year = (timeMod.year << 4) | timeBuf.year/10;	//BCD高位
	timeMod.year = (timeMod.year << 4) | timeBuf.year%10;	//BCD低位

	timeMod.month = (timeMod.month << 4) | timeBuf.month/10;	//BCD高位
	timeMod.month = (timeMod.month << 4) | timeBuf.month%10;	//BCD低位

	timeMod.day = (timeMod.day << 4) | timeBuf.day/10;	//BCD高位
	timeMod.day = (timeMod.day << 4) | timeBuf.day%10;	//BCD低位

	timeMod.week = (timeMod.week << 4) | timeBuf.week/10;	//BCD高位
	timeMod.week = (timeMod.week << 4) | timeBuf.week%10;	//BCD低位

	timeMod.hour = (timeMod.hour << 4) | timeBuf.hour/10;	//BCD高位
	timeMod.hour = (timeMod.hour << 4) | timeBuf.hour%10;	//BCD低位

	timeMod.min = (timeMod.min << 4) | timeBuf.min/10;	//BCD高位
	timeMod.min = (timeMod.min << 4) | timeBuf.min%10;	//BCD低位

	timeMod.sec = (timeMod.sec << 4) | timeBuf.sec/10;	//BCD高位
	timeMod.sec = (timeMod.sec << 4) | timeBuf.sec%10;	//BCD低位

	DS1302BurstWrite(&timeMod);
}

void ShowAdjusted()				  
{
	uint8 pdata strTmp[2];
	uint16 pdata CNTmp[2];

	if(mMode == SetDate)
	{
		if(curPos < 7){
			LCDClearArea(5,0);
			LCDDrawArea(5,0,SaveButton); //数据已修改，save图标复原	
		}
		switch(curPos){
			case 0:
				strTmp[0] = (timeBuf.year /10) + '0';
				strTmp[1] = (timeBuf.year %10) + '0';
				LCDShowStr(2,1,strTmp,2);
				LCDSetCursor(2,1);	 //Cursor保持不动
				break;
			case 1:
				strTmp[0] = (timeBuf.month /10) + '0';
				strTmp[1] = (timeBuf.month %10) + '0';
				LCDShowStr(4,1,strTmp,2);
				LCDSetCursor(4,1);	 //Cursor保持不动
				break;
			case 2:
				strTmp[0] = (timeBuf.day /10) + '0';
				strTmp[1] = (timeBuf.day %10) + '0';
				LCDShowStr(6,1,strTmp,2);
				LCDSetCursor(6,1);	 //Cursor保持不动
				break;
			case 3:
				LCDShowCN(3,2,weekCN[timeBuf.week],2);
				LCDSetCursor(4,2);	 //Cursor保持不动
				break;			
			case 4:
				CNTmp[0] = (timeBuf.hour /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.hour %10) + 0xA3B0;
				LCDShowCN(0,3,CNTmp,2);
				LCDSetCursor(1,3);	 //Cursor保持不动
				break;
			case 5:
				CNTmp[0] = (timeBuf.min /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.min %10) + 0xA3B0;
				LCDShowCN(3,3,CNTmp,2);
				LCDSetCursor(4,3);	 //Cursor保持不动
				break;
			case 6:
				CNTmp[0] = (timeBuf.sec /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.sec %10) + 0xA3B0;
				LCDShowCN(6,3,CNTmp,2);
				LCDSetCursor(7,3);	 //Cursor保持不动
				break;
			case 7:				   	//保存图标更改为已保存
				CNTmp[0] = 0xA1CC;
				LCDClearImage(5,0);
				LCDShowCN(5,0,CNTmp,1);
				LCDSetCursor(5,0);
			default: break;
		}
	}else if(mMode == SetClock){
		switch(curPos){
			case 0:
				CNTmp[0] = (cHour / 10) + 0xA3B0;
				CNTmp[1] = (cHour % 10) + 0xA3B0;
				LCDShowCN(2,2,CNTmp,2);
				LCDSetCursor(3,2);	 //Cursor保持不动
				break;
			case 1:
				CNTmp[0] = (cMin / 10) + 0xA3B0;
				CNTmp[1] = (cMin % 10) + 0xA3B0;
				LCDShowCN(5,2,CNTmp,2);
				LCDSetCursor(6,2);	 //Cursor保持不动
				break;
			case 2:
				if(bClockOpen){
					LCDDrawArea(5,0,OpenVolume);//显示开启闹钟图标
				}else{
					LCDDrawArea(5,0,StopVolume);//显示关闭闹钟图标
				}
				break;
			default: break;
		}
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
		case 7:
			SaveTime();
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
		case 2:	  //是否开启闹钟
			bClockOpen = ~bClockOpen;
			break;
			
		default: break;
	}
	ShowAdjusted();
}

void GetDecimalTime()
{
	timeBuf.year = (timeMod.year >> 4)*10 + (timeMod.year & 0x0F);
	timeBuf.month = (timeMod.month >> 4)*10 + (timeMod.month & 0x0F);
	timeBuf.day = (timeMod.day >> 4)*10 + (timeMod.day & 0x0F);
	timeBuf.week =  timeMod.week;
	timeBuf.hour = (timeMod.hour >> 4)*10 + (timeMod.hour & 0x0F);
	timeBuf.min = (timeMod.min >> 4)*10 + (timeMod.min & 0x0F);
	timeBuf.sec = (timeMod.sec >> 4)*10 + (timeMod.sec & 0x0F);
}

void KeyAction(uint8 keyCode)
{
	if(keyCode == 0x0D)//按回车键进入设定状态
	{
		uint16 pdata clockStr[5];

		if(mMode == SetDate){
			mMode = SetClock;
			LCDClearAll();
			clockStr[0] = 0xC4D6;//"闹钟设定："
			clockStr[1] = 0xD6D3;
			clockStr[2] = 0xC9E8;
			clockStr[3] = 0xB6A8;
			clockStr[4] = 0xA3BA;
			LCDShowCN(0,0,clockStr,5);
			LCDClearImage(5,0);	 //清除太阳、月亮图标
			LCDClearImage(0,1);	 //	清除小闹钟图标
			if(bClockOpen)	//Clock是否开启？
			{
				LCDDrawArea(5,0,OpenVolume);
			}else{
				LCDDrawArea(5,0,StopVolume);
			}
			clockStr[0] = cHour/10 + 0xA3B0;
			clockStr[1] = cHour%10 + 0xA3B0;
			clockStr[2] = 0xA1C3;
			clockStr[3] = cMin/10 + 0xA3B0;
			clockStr[4] = cMin%10 + 0xA3B0;		 //“00:00”
			LCDShowCN(2,2,clockStr,5);
			curPos = 0;
			LCDSetCursor(3,2);
			LCDShowCursor();
		}else{
			mMode = SetDate;
			LCDClearAll();
			clockStr[0] = 0xD0A3;  //"校对时间："
			clockStr[1]	= 0xB6D4;
			clockStr[2] = 0xCAB1;
			clockStr[3] = 0xBCE4;
			clockStr[4]	= 0xA3BA;
			LCDShowCN(0,0,clockStr,5);//为节省内存借clockStr一用
			LCDClearImage(5,0);	//清除响铃图标
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
		mMode = ReadDate;
		LCDClearAll();	  //清屏
		LCDCancelCursor();	//取消光标闪烁
		LCDClearImage(5,0);	  //清除小图标
		pSec = 0xAA;
		pHour = 0xAA;
		pDay = 0xAA;
		ShowCurrentTime(); //强制时间更新界面
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