#include "config.h"
#include "ds1302.h"
#include "lcd12864.h"
#include "lunar.h"
#include "DHT11.h"

uint16 code weekCN[8][2] = {
	{0x0000,0x0000},{0xD6DC,0xD2BB},
	{0xD6DC,0xB6FE},{0xD6DC,0xC8FD},
	{0xD6DC,0xCBC4},{0xD6DC,0xCEE5},
	{0xD6DC,0xC1F9},{0xD6DC,0xC8D5}
};	/*�ա���һ~����*/

void ShowCurrentTime()
{
	static uint8 pSec = 0xAA;
	static uint8 pDay = 0xAA;
	static uint8 pHour = 0xAA;
	static bit bDay = 0;
	uint16 pdata tStr[8];
	uint8  pdata dStr[16];
	uint16 pdata dSplit;

	DS1302BurstRead(&timeMod); 
	if(pSec != timeMod.sec)
	{
		pSec = timeMod.sec;
		tStr[0] = (timeMod.hour >> 4) + 0xA3B0;	//ʱ��
		tStr[1] = (timeMod.hour & 0x0F) + 0xA3B0;//ʱ��
		tStr[2] = 0xA1C3;//ʱ��ָ���
		tStr[3] = (timeMod.min >> 4) + 0xA3B0;	//�ָ�
		tStr[4] = (timeMod.min & 0x0F) + 0xA3B0;//�ֵ�
		tStr[5] = 0xA1C3;//ʱ��ָ���
		tStr[6] = (timeMod.sec >> 4) + 0xA3B0;	//���
		tStr[7] = (timeMod.sec & 0x0F) + 0xA3B0;//��� 
		LCDShowCN(0,3,tStr,8);//Һ����1����ʾ����ʱ��
		if(pHour != timeMod.hour){
			pHour = timeMod.hour;
			if(timeMod.hour >= 0x08 && timeMod.hour <= 0x18)	//���ڰ��죬��ʾ̫��ͼ��
			{
				if(!bDay)
				{
					bDay = ~bDay;
					LCDDrawArea(5,0,sunImage);
				}
			}else{	  //���ں�ҹ����ʾ����ͼ��
				if(!bDay)
				{
					bDay = ~bDay;
					LCDDrawArea(5,0,sunImage);
				}
			}
		}
	}

	if(pDay != timeMod.day)	//�����ڸı���
	{
		pDay = timeMod.day;	  //��������
		dStr[0] = ' ';
		dStr[1] = ' ';
		dStr[2] = '2';
		dStr[3] = '0';
		dStr[4] = (timeMod.year >> 4) + '0';	//���
		dStr[5] = (timeMod.year & 0x0F) + '0';//���
		dStr[6] = ' ';
		dStr[7] = ' ';
		dStr[8] = (timeMod.month >> 4) + '0';	//�¸�
		dStr[9] = (timeMod.month & 0x0F) + '0';//�µ�
		dStr[10] = ' ';
		dStr[11] = ' ';
		dStr[12] = (timeMod.day >> 4) + '0';	//�ո�
		dStr[13] = (timeMod.day & 0x0F) + '0';//�յ�
		dSplit = 0xA1A4;

		LCDShowStr(0,1,dStr,14);//Һ����2����ʾ����ʱ��
		LCDShowCN(3,1,&dSplit,1);
		LCDShowCN(5,1,&dSplit,1);	//��ʾ���ڷָ���
		LCDShowCN(3,2,weekCN[timeMod.week],2);	//Һ����3����ʾ����

	 /*	uint16 lunar[7];
	uint8  lunarLen, lday, lyear, lmonth;*/
	/*	lyear = (timeMod.year >> 4) * 10 + (timeMod.year & 0x0F);
		lmonth = (timeMod.month >> 4) * 10 + (timeMod.month & 0x0F);
		lday = (timeMod.day >> 4) * 10 + (timeMod.day & 0x0F); 
		lunarLen = GetLunarDate(lyear, lmonth, lday, lunar);
		LCDShowCN(1,2,lunar,lunarLen);	*/
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