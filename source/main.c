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
	
	/*************** �������� *****************/
	if(!flagStopAlarm){				//�û�δȡ��
		if(!flagBuzzOn){		 //	����δ��
			if(cHour == (timeMod.hour >> 4)*10+(timeMod.hour&0x0F))	//����Ƿ������
			{
				if(cMin == (timeMod.min >> 4)*10+(timeMod.min&0x0F))
				{
					flagBuzzOn = 1;
				}
			 }
		 }
	}else{					   //�û�ȡ��
		flagBuzzOn = 0;				 
		alarmCnt++;
		if(alarmCnt >= 300) //һ���Ӻ�ȡ���û��趨
		{
			alarmCnt = 0;
			flagStopAlarm = 0;
		}
	}
}