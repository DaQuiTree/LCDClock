#define _MAIN_C
#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "keyboard.h"
#include "time.h"
#include "DHT11.h"
#include "keyboard.h"
#include "main.h"

SystemMode mMode = ReadDate;
bit flagStopAlarm = 0;

void CheckClock();

void main()
{
	EA = 1;	
	InitDS1302();										  
	InitLCD12864();
	LCDInitImage();
	ConfigTimerZero(1);
	
	while(1)
	{
		if(mMode == ReadDate){
			if(flag200ms){
				flag200ms = 0;
				ShowCurrentTime();
				if(bClockOpen){
					CheckClock();
				}
			}else if(mMode == ReadInfo){
				if(flag1s){
					flag1s = 0;
					ShowTemp();
				} 
			} 
		}
		KeyDriver();
	}
}

void CheckClock()
{
	static uint16 alarmCnt = 0;
//	static bit flagTimeArrive = 0;
	
	/*************** 启动闹钟 *****************/
	if(!flagStopAlarm){				//用户未取消
		if(!flagBuzzOn){		 //	闹铃未响
			if(cHour == (timeMod.hour >> 4)*10+(timeMod.hour&0x0F))	//检测是否该响了
			{
				if(cMin == (timeMod.min >> 4)*10+(timeMod.min&0x0F))
				{
					flagBuzzOn = 1;
				}
			 }
		 }
	}else{					   //用户取消
		flagBuzzOn = 0;				 
		alarmCnt++;
		if(alarmCnt >= 300) //一分钟后取消用户设定
		{
			alarmCnt = 0;
			flagStopAlarm = 0;
		}
	}
}