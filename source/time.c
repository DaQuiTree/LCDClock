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
};	/*�ա���һ~����*/

void ClearInforArea()  //�����Ļ������,����������
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
				LCDDrawArea(5,0,sunImage);
			}else{	  //���ں�ҹ����ʾ����ͼ��
				LCDDrawArea(5,0,sunImage);
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
	LCDShowCN(3,1,dat,3);  //�¶�

	dat[0] = humi/10 + 0xA3B0;
	dat[1] = humi%10 + 0xA3B0;
	dat[2] = 0xA3A5; 
	LCDShowCN(3,2,dat,3);  //ʪ��
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
	
	LCDClearAll();	  //�����ʾ����
	LCDShowCN(0,1,iPrompt[0],3);  //p�¶ȣ�
	LCDShowCN(0,2,iPrompt[1],3);  //pʪ�ȣ�
	LCDShowCN(0,3,iPrompt[2],3);  //p���ӣ�
	ShowTemp();				   //��ʾ��ֵ
	lyear = (timeMod.year >> 4) * 10 + (timeMod.year & 0x0F);
	lmonth = (timeMod.month >> 4) * 10 + (timeMod.month & 0x0F);
	lday = (timeMod.day >> 4) * 10 + (timeMod.day & 0x0F); 
	lunarLen = GetLunarDate(lyear, lmonth, lday, lunar);
	LCDShowCN(1,0,lunar,lunarLen);
}

void KeyAction(uint8 keyCode)
{
	if(keyCode == 0x0D)//�س�
	{
		mMode++;
		mMode &= 0x01;//�ı�mMode״̬
		if(mMode == ReadDate){
			LCDClearAll();	//����
		   	pSec = 0xAA;	//ǿ����ʾʱ��
			pDay = 0xAA;
			pHour = 0xAA;
		}else if(mMode == ReadInfo){
			ShowInfo();
		}
	}else if(keyCode == 0x1B){//ȡ��

	}else if(keyCode == 0x26){//����

	}else if(keyCode == 0xD0){
			LCDClearAll();	//����
			mMode 	
	}
}