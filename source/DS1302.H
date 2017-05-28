/*
 * DS1302.h
 * --------
 * func：声明DS1302的接口函数以及全局变量
 */

 #ifndef _DS1302_H
 #define _DS1302_H

 struct sTime{
	uint8 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint8 week;
	uint8 am;
};

 #ifndef _DS1302_C

 extern struct sTime timeMod;

 #endif

 void InitDS1302();
 void DS1302BurstWrite(struct sTime* time);
 void DS1302BurstRead(struct sTime* time);

 #endif
