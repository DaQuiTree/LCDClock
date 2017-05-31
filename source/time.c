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

struct sTime timeBuf; //�洢��ʮ����ʱ��

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

	if(pHour == 0xAA) //��ʾС����ͼ���ʱ������ʾ̫��/����ʱ����ͬ
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
				LCDDrawArea(5,0,moonImage);
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
	if(bClockOpen)	   //Ϊ��ʡ�ڴ����lunar   
	{
		lunar[0] = cHour/10 + 0xA3B0; //��ʾ���Ӿ���ʱ��
		lunar[1] = cHour%10 + 0xA3B0;
		lunar[2] = 0xA1C3;			
		lunar[3] = cMin/10 + 0xA3B0;
		lunar[4] = cMin%10 + 0xA3B0;
		LCDShowCN(3,3,lunar,5);
	}else{						
		lunar[0] = 0xCEB4;	// ��ʾ��δ���á�
		lunar[1] = 0xBFAA;		
		lunar[2] = 0xC6F4;		 
		LCDShowCN(3,3,lunar,3);	  
	}
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
	timeMod.year = (timeMod.year << 4) | timeBuf.year/10;	//BCD��λ
	timeMod.year = (timeMod.year << 4) | timeBuf.year%10;	//BCD��λ

	timeMod.month = (timeMod.month << 4) | timeBuf.month/10;	//BCD��λ
	timeMod.month = (timeMod.month << 4) | timeBuf.month%10;	//BCD��λ

	timeMod.day = (timeMod.day << 4) | timeBuf.day/10;	//BCD��λ
	timeMod.day = (timeMod.day << 4) | timeBuf.day%10;	//BCD��λ

	timeMod.week = (timeMod.week << 4) | timeBuf.week/10;	//BCD��λ
	timeMod.week = (timeMod.week << 4) | timeBuf.week%10;	//BCD��λ

	timeMod.hour = (timeMod.hour << 4) | timeBuf.hour/10;	//BCD��λ
	timeMod.hour = (timeMod.hour << 4) | timeBuf.hour%10;	//BCD��λ

	timeMod.min = (timeMod.min << 4) | timeBuf.min/10;	//BCD��λ
	timeMod.min = (timeMod.min << 4) | timeBuf.min%10;	//BCD��λ

	timeMod.sec = (timeMod.sec << 4) | timeBuf.sec/10;	//BCD��λ
	timeMod.sec = (timeMod.sec << 4) | timeBuf.sec%10;	//BCD��λ

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
			LCDDrawArea(5,0,SaveButton); //�������޸ģ�saveͼ�긴ԭ	
		}
		switch(curPos){
			case 0:
				strTmp[0] = (timeBuf.year /10) + '0';
				strTmp[1] = (timeBuf.year %10) + '0';
				LCDShowStr(2,1,strTmp,2);
				LCDSetCursor(2,1);	 //Cursor���ֲ���
				break;
			case 1:
				strTmp[0] = (timeBuf.month /10) + '0';
				strTmp[1] = (timeBuf.month %10) + '0';
				LCDShowStr(4,1,strTmp,2);
				LCDSetCursor(4,1);	 //Cursor���ֲ���
				break;
			case 2:
				strTmp[0] = (timeBuf.day /10) + '0';
				strTmp[1] = (timeBuf.day %10) + '0';
				LCDShowStr(6,1,strTmp,2);
				LCDSetCursor(6,1);	 //Cursor���ֲ���
				break;
			case 3:
				LCDShowCN(3,2,weekCN[timeBuf.week],2);
				LCDSetCursor(4,2);	 //Cursor���ֲ���
				break;			
			case 4:
				CNTmp[0] = (timeBuf.hour /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.hour %10) + 0xA3B0;
				LCDShowCN(0,3,CNTmp,2);
				LCDSetCursor(1,3);	 //Cursor���ֲ���
				break;
			case 5:
				CNTmp[0] = (timeBuf.min /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.min %10) + 0xA3B0;
				LCDShowCN(3,3,CNTmp,2);
				LCDSetCursor(4,3);	 //Cursor���ֲ���
				break;
			case 6:
				CNTmp[0] = (timeBuf.sec /10) + 0xA3B0;
				CNTmp[1] = (timeBuf.sec %10) + 0xA3B0;
				LCDShowCN(6,3,CNTmp,2);
				LCDSetCursor(7,3);	 //Cursor���ֲ���
				break;
			case 7:				   	//����ͼ�����Ϊ�ѱ���
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
				LCDSetCursor(3,2);	 //Cursor���ֲ���
				break;
			case 1:
				CNTmp[0] = (cMin / 10) + 0xA3B0;
				CNTmp[1] = (cMin % 10) + 0xA3B0;
				LCDShowCN(5,2,CNTmp,2);
				LCDSetCursor(6,2);	 //Cursor���ֲ���
				break;
			case 2:
				if(bClockOpen){
					LCDDrawArea(5,0,OpenVolume);//��ʾ��������ͼ��
				}else{
					LCDDrawArea(5,0,StopVolume);//��ʾ�ر�����ͼ��
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
		case 0:			//����Clockʱ
			cHour++;
			if(cHour > 23) cHour = 0;
			break;
		case 1:			 //����Clock��
			cMin++;
			if(cMin > 59) cMin = 0;
			break;
		case 2:	  //�Ƿ�������
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
	if(keyCode == 0x0D)//���س��������趨״̬
	{
		uint16 pdata clockStr[5];

		if(mMode == SetDate){
			mMode = SetClock;
			LCDClearAll();
			clockStr[0] = 0xC4D6;//"�����趨��"
			clockStr[1] = 0xD6D3;
			clockStr[2] = 0xC9E8;
			clockStr[3] = 0xB6A8;
			clockStr[4] = 0xA3BA;
			LCDShowCN(0,0,clockStr,5);
			LCDClearImage(5,0);	 //���̫��������ͼ��
			LCDClearImage(0,1);	 //	���С����ͼ��
			if(bClockOpen)	//Clock�Ƿ�����
			{
				LCDDrawArea(5,0,OpenVolume);
			}else{
				LCDDrawArea(5,0,StopVolume);
			}
			clockStr[0] = cHour/10 + 0xA3B0;
			clockStr[1] = cHour%10 + 0xA3B0;
			clockStr[2] = 0xA1C3;
			clockStr[3] = cMin/10 + 0xA3B0;
			clockStr[4] = cMin%10 + 0xA3B0;		 //��00:00��
			LCDShowCN(2,2,clockStr,5);
			curPos = 0;
			LCDSetCursor(3,2);
			LCDShowCursor();
		}else{
			mMode = SetDate;
			LCDClearAll();
			clockStr[0] = 0xD0A3;  //"У��ʱ�䣺"
			clockStr[1]	= 0xB6D4;
			clockStr[2] = 0xCAB1;
			clockStr[3] = 0xBCE4;
			clockStr[4]	= 0xA3BA;
			LCDShowCN(0,0,clockStr,5);//Ϊ��ʡ�ڴ��clockStrһ��
			LCDClearImage(5,0);	//�������ͼ��
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
		mMode = ReadDate;
		LCDClearAll();	  //����
		LCDCancelCursor();	//ȡ�������˸
		LCDClearImage(5,0);	  //���Сͼ��
		pSec = 0xAA;
		pHour = 0xAA;
		pDay = 0xAA;
		ShowCurrentTime(); //ǿ��ʱ����½���
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