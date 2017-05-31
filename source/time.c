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
uint8 pHour = 0xAA; //ʱ�ӵĳ�ʼֵ

uint8 cHour = 0x00;
uint8 cMin = 0x00; //���ӳ�ʼֵ

sTime timeBuf; //�洢��ʮ����ʱ��

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
		case 0:			//������
			timeBuf.year++;
			if(timeBuf.year > 99) timeBuf.year = 0;
			break;
		case 1:			 //������
			timeBuf.month++;
			if(timeBuf.month > 12) timeBuf.month = 1;
			break;
		case 2:			//��
			timeBuf.day++;
			if(timeBuf.year % 4 == 0){	//������µ�������
				if(timeBuf.month == 2){
					if(timeBuf.day > 29) timeBuf.day = 1;
					break;
				}; 
			}
			if(timeBuf.day > DateMap[timeBuf.month-1][1]) timeBuf.day = 1;
			break;
		case 3:		   //��
			timeBuf.week++;
			if(timeBuf.week > 7) timeBuf.week = 1;
			break;
		case 4:		   //ʱ
			timeBuf.hour++;
			if(timeBuf.hour > 23) timeBuf.hour = 0;
			break;
		case 5:		   //��
			timeBuf.min++;
			if(timeBuf.min > 59) timeBuf.min = 0;
			break;
		case 6:		   //��
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
		case 0:			//����Clockʱ
			cHour++;
			if(cHour > 23) cHour = 0;
			break;
		case 1:			 //����Clock��
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
	if(keyCode == 0x0D)//���س��������趨״̬
	{
		uint16 pdata clockStr[5];

		if(mMode == SetDate){
			mMode = SetClock;
			LCDClearAll();
			LCDShowCN(0,0,"�����趨��",5);
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
			LCDShowCN(2,0,clockStr,2);//Ϊ��ʡ�ڴ��clockStrһ��
			LCDDrawArea(5,0,SaveButton);
			pSec = 0xAA;
			pDay = 0xAA; //ǿ��ˢ��ʱ����棨��ˢ��̫������ͼ�꣩
			ShowCurrentTime();
			GetDecimalTime();//��BCDʱ��ת��Ϊʮ����ʱ��
			curPos = 0;
			LCDSetCursor(2,1);
			LCDShowCursor();
			
		}
	}else if(keyCode == 0x1B){//ȡ��

	}else if(keyCode == 0x26){//����
		if(mMode == SetDate){
			AdjustDate();
		}else if(mMode == SetClock){
			AdjustClock();
		}		
	}else if(keyCode == 0x27){//���ҡ���ҳ
		if(mMode < SetDate) //������Ϣչʾ״̬ʱ��ҳ
		{
			mMode++;
			mMode &= 0x01;//�ı�mMode״̬
			if(mMode == ReadDate){ //ʱ�����
				LCDClearAll();	//����
			   	pSec = 0xAA;	//ǿ����ʾʱ��
				pDay = 0xAA;
				pHour = 0xAA;
			}else if(mMode == ReadInfo){//�¶ȵ���Ϣ����
				ShowInfo();
			}
		}else{	 //�����趨״̬�Ҽ��൱�ڵ����趨λ��
			if(mMode == SetDate){
				RightShiftDate();
			}else if(mMode == SetClock){
				RightShiftClock();
			}
		}		
	}
}