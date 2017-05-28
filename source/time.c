#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "lunar.h"

uint16* code weekCN[] = {
	"*","һ","��","��","��","��","��","��"
};

uint8 ClockGetLunar(struct sTime* curTime, uint16* lunarDate)
{
	uint32 year,month,day;
	
	year = (curTime->year & 0xF0) * 10 + (curTime->year >> 4);
	month = (curTime->month & 0xF0) * 10 + (curTime->month >> 4);
	day = (curTime->day & 0xF0) * 10 + (curTime->day >> 4); 
	return(GetLunarDate(year, month, day, lunarDate));
}

void ShowCurrentTime()
{
	static uint8 pSec = 0xAA;
	static uint8 pDay = 0xAA;
	uint8  tStr[20];
	uint8  lunarLen;
	uint16 lunar[8];

	DS1302BurstRead(&timeMod); 
	if(pSec != timeMod.sec)
	{
		pSec = timeMod.sec;
		tStr[0] = ' ';
		tStr[1] = (timeMod.month >> 4) + '0';	//�¸�
		tStr[2] = (timeMod.month & 0x0F) + '0';//�µ�
		tStr[3] = '-';//���ڷָ���
		tStr[4] = (timeMod.day >> 4) + '0';	//�ո�
		tStr[5] = (timeMod.day & 0x0F) + '0';//�յ�
		tStr[6] = ' '; 
		tStr[7] = (timeMod.hour >> 4) + '0';	//ʱ��
		tStr[8] = (timeMod.hour & 0x0F) + '0';//ʱ��
		tStr[9] = ':';//ʱ��ָ���
		tStr[10] = (timeMod.min >> 4) + '0';	//�ָ�
		tStr[11] = (timeMod.min & 0x0F) + '0';//�ֵ�
		tStr[12] = ':';//ʱ��ָ���
		tStr[13] = (timeMod.sec >> 4) + '0';	//���
		tStr[14] = (timeMod.sec & 0x0F) + '0';//���
		LCDShowStr(0,1,tStr,15);//Һ����1����ʾ����ʱ��
	}
/*	if(pDay != timeMod.day)	//��ʾ�ꡢ��ݡ�ũ��
	{
		pDay = timeMod.day;
		tStr[0] = '2';	//��Ԫ2000��
		tStr[1] = '0';
		tStr[2] = (timeMod.year >> 4) + '0';	//���
		tStr[3] = (timeMod.year & 0x0F) + '0';//���
		LCDShowStr(1,0,tStr,4);//��0����ʾ���
		LCDShowCN(4,0,"���",2);
		LCDShowCN(6,0,weekCN[timeMod.week],1);
		lunarLen = ClockGetLunar(&timeMod, lunar);
		LCDShowCN(1,2,lunar,lunarLen);
	}  */
}