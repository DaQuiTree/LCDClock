#define _LCD12864_C
#include "config.h"
#include "lcd12864.h"

uint8 code clockImage[32] ={
	0x3C,0x1C,0x7C,0x1E,0xFF,0xFF,0xFF,0xFF,0xFE,0x3F,0xF9,0x8F,0x79,0x8E,0x71,0x86,
	0x71,0xF6,0x71,0xF6,0x78,0x0E,0x38,0x0E,0x3E,0x3C,0x1F,0xFC,0x1F,0xFC,0x1C,0x1C,
};

uint8 code sunImage[32] = {
	0x00,0x00,0x01,0x00,0x41,0x04,0x20,0x08,0x13,0x90,0x07,0xC0,0x0F,0xE0,0xDF,0xF6,
	0x1F,0xF0,0x0F,0xE0,0x07,0xC0,0x13,0x90,0x20,0x08,0x41,0x04,0x01,0x00,0x00,0x00,
};

uint8 code moonImage[32] = {
	0x00,0x00,0x07,0x80,0x03,0xC0,0x01,0xE0,0x01,0xF0,0x00,0xF8,0x00,0xF8,0x00,0xF8,
	0x00,0xF8,0x00,0xF8,0x01,0xF0,0x03,0xF0,0x07,0xE0,0x0F,0xC0,0x1F,0x00,0x00,0x00,
};

void LCDWaitReady()
{
	uint8 tmp = 0xFF;

	LCD12864_RS = 0;
	LCD12864_RW = 1;
	do{
		LCD12864_EN = 1;
		tmp = LCD12864_DB & 0x80;
		LCD12864_EN = 0;	
	}while(tmp != 0x00);
	LCD12864_EN = 0;	
}

void LCDWriteCmd(uint8 cmd)
{
	LCDWaitReady();
	LCD12864_RS = 0;
	LCD12864_RW = 0;
	LCD12864_EN = 1;
	LCD12864_DB = cmd;
	LCD12864_EN = 0;	
}

void LCDWriteData(uint8 dat)
{
	LCDWaitReady();
	LCD12864_RS = 1;
	LCD12864_RW = 0;
	LCD12864_EN = 1;
	LCD12864_DB = dat;
	LCD12864_EN = 0;
}

void InitLCD12864()
{
	LCDWriteCmd(0x30);
	LCDWriteCmd(0x0C);
	LCDWriteCmd(0x01); //������(0,0)λ���ַ��趨�ɿո�
	LCDShowStr(0,0," ",1);
}

void LCDShowStr(uint8 x, uint8 y, uint8* str, uint8 strlen)	//д���ַ���
{
	LCDSetCursor(x,y);
	do{
		LCDWriteData(*str++);
		strlen--;	
	}while(strlen > 0);
}

void LCDShowCN(uint8 x, uint8 y, uint16* str, uint8 strlen)	//д���ַ���
{
	uint8 msb,lsb;

	while(strlen-- > 0){
		LCDSetCursor(x,y);
		x++;
		if(x > 7){
			x = 0;
			y++;
		} 			//��д���ַ
		msb = (uint8)(*str >> 8);	//д������
		lsb = (uint8)(*str); 
		LCDWriteData(msb);
		LCDWriteData(lsb);
		str++;						
	}
}

void LCDSetCursor(uint8 x, uint8 y)
{
	uint8 pos = 0;

	switch(y)
	{
		case 0:
			pos = 0x80 + x;
			break;
		case 1:
			pos = 0x90 + x;
			break;
		case 2:
			pos = 0x88 + x;
			break;
		case 3:
			pos = 0x98 + x;
			break;
		default:
		 	break;
	}
	LCDWriteCmd(pos);
}

void LCDShowCursor()
{
	LCDWriteCmd(0x0F);
}

void LCDCancelCursor()
{
	LCDWriteCmd(0x0C);
}

void LCDClearAll()
{
	LCDWriteCmd(0x30);
	LCDWriteCmd(0x01);
	LCDShowStr(0,0," ",1);
}

void LCDShowImage(uint8* image)
{
	uint8 i,j;

	LCDWriteCmd(0x34); //����ָ��趨�رջ�ͼ��ʾ
	LCDWriteCmd(0x34);
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //���û�ͼ��ֱ��ַ
		LCDWriteCmd(0x80);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(*image++);
			LCDWriteData(*image++);
		}
	}
	
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //���û�ͼ��ֱ��ַ
		LCDWriteCmd(0x88);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(*image++);
			LCDWriteData(*image++);
		}
	}	
	LCDWriteCmd(0x36); //�򿪻�ͼ��ʾ
	LCDWriteCmd(0x30);	//�������ָ�����
}

void LCDDrawArea(uint8 x, uint8 y, uint8* image)//��ʾ16*16ͼƬ
{
	uint8 i;

	LCDWriteCmd(0x34); //����ָ��趨�رջ�ͼ��ʾ
	LCDWriteCmd(0x34);

	if(y < 2){			//����ǰ����
		for(i = 0; i < 16; i++){
			LCDWriteCmd((0x80 | y*16)|i); //���û�ͼ��ֱ��ַ
			LCDWriteCmd(0x80 | x);//���û�ͼˮƽ��ַ

			LCDWriteData(*image++);// д��һ��
			LCDWriteData(*image++);
		}
	}else{			//���ں�����
		for(i = 0; i < 16; i++){
			LCDWriteCmd((0x80 | y*16)|i); //���û�ͼ��ֱ��ַ
			LCDWriteCmd(0x88 | x);//���û�ͼˮƽ��ַ

			LCDWriteData(*image++);// д��һ��
			LCDWriteData(*image++);
		}
	}

	LCDWriteCmd(0x80 | y*16); //����ԭ��
	LCDWriteCmd(0x80 | x);

	LCDWriteCmd(0x36);
	LCDWriteCmd(0x30);
}

void LCDInitImage()
{
	uint8 i,j;

	LCDWriteCmd(0x34); //����ָ��趨�رջ�ͼ��ʾ
	LCDWriteCmd(0x34);
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //���û�ͼ��ֱ��ַ
		LCDWriteCmd(0x80);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(0x00);
			LCDWriteData(0x00);
		}
	}
	
	for(i = 0; i < 32; i++){
		LCDWriteCmd(0x80 | i); //���û�ͼ��ֱ��ַ
		LCDWriteCmd(0x88);
		for( j = 0; j < 8; j++)
		{
			LCDWriteData(0x00);
			LCDWriteData(0x00);
		}
	}	
	LCDWriteCmd(0x36);
	LCDWriteCmd(0x30);		
}

void LCDClearImage(uint8 x, uint8 y)
{
	uint8 i;

	LCDWriteCmd(0x34); //����ָ��趨�رջ�ͼ��ʾ
	LCDWriteCmd(0x34);

	if(y < 2){			//����ǰ����
		for(i = 0; i < 16; i++){
			LCDWriteCmd((0x80 | y*16)|i); //���û�ͼ��ֱ��ַ
			LCDWriteCmd(0x80 | x);//���û�ͼˮƽ��ַ

			LCDWriteData(0x00);// д��һ��
			LCDWriteData(0x00);
		}
	}else{			//���ں�����
		for(i = 0; i < 16; i++){
			LCDWriteCmd((0x80 | y*16)|i); //���û�ͼ��ֱ��ַ
			LCDWriteCmd(0x88 | x);//���û�ͼˮƽ��ַ

			LCDWriteData(0x00);// д��һ��
			LCDWriteData(0x00);
		}
	}

	LCDWriteCmd(0x80 | y*16); //����ԭ��
	LCDWriteCmd(0x80 | x);

	LCDWriteCmd(0x36);
	LCDWriteCmd(0x30);	
}

void LCDClearArea(uint8 x, uint8 y)
{
	uint16 code clearDat[] = {
		0xA1A0,0xA1A0,0xA1A0,0xA1A0,0xA1A0,0xA1A0,0xA1A0,0xA1A0,
	};

	LCDShowCN(x,y,clearDat,8-x);
}