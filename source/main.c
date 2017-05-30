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
			}
		}else if(mMode == ReadInfo){
			if(flag5s)
			{
				flag5s = 0;
				ShowTemp();
			} 
		} 
		KeyDriver();
	}
}