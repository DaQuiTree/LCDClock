#define _PWMLED_C

#include "config.h"
#include "pwmled.h"

ledState mLedState = LED_Breath;

uint8 HighRH = 0;  //�ߵ�ƽ����ֵ�ĸ��ֽ�
uint8 HighRL = 0;  //�ߵ�ƽ����ֵ�ĵ��ֽ�
uint8 LowRH  = 0;  //�͵�ƽ����ֵ�ĸ��ֽ�
uint8 LowRL  = 0;  //�͵�ƽ����ֵ�ĵ��ֽ�

uint32 PeriodCnt = 0;

void ConfigTimerTwo() //��ʱ���� 50ms
{

	RCAP2H = (uint8)((65535 - SYS_CLK/1000*50 + 12) >> 8); //���ö�ʱ����Ϊ��ʾ������ʱ��
	RCAP2L = (uint8)(65535 - SYS_CLK/1000*50 + 12);

	T2CON = 0x00;

	TH2 = RCAP2H;
	TL2 = RCAP2L;

	TR2 = 0; 	// T2�ж���ʱ�رգ��ȴ�����
	//PT2 = 1;//����LED��ʾ��������ж����ȼ�
	ET2 = 0;
}

void ConfigPWM(uint16 fr, uint8 dc)
{
    uint16 high, low;
    
    PeriodCnt = SYS_CLK / fr; //����һ����������ļ���ֵ
    high = (PeriodCnt*dc) / 100;    //����ߵ�ƽ����ļ���ֵ
    low  = PeriodCnt - high;        //����͵�ƽ����ļ���ֵ
    high = 65536 - high + 12;       //����ߵ�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    low  = 65536 - low  + 12;       //����͵�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    HighRH = (uint8)(high>>8); //�ߵ�ƽ����ֵ���Ϊ�ߵ��ֽ�
    HighRL = (uint8)high;
    LowRH  = (uint8)(low>>8);  //�͵�ƽ����ֵ���Ϊ�ߵ��ֽ�
    LowRL  = (uint8)low;
    TMOD &= 0x0F;   //����T1�Ŀ���λ
    TMOD |= 0x10;   //����T1Ϊģʽ1
    TH1 = HighRH;   //����T1����ֵ
    TL1 = HighRL;
    ET1 = 0;        //�ر�T1�жϣ��ȴ�����
    TR1 = 0;        //�ر�T1
    PWMOUT = 1;     //����ߵ�ƽ
}
void AdjustDutyCycle(uint8 dc)
{
	uint16 high, low;

	high = (PeriodCnt*dc)/100;
	low = PeriodCnt - high;
	high = 65536 - high + 12;
	low = 65536 - low + 12;
	HighRH = (uint8)(high >> 8);
	HighRL = (uint8)high;
	LowRH = (uint8)(low >> 8);
	LowRL = (uint8)low;
}
void InterruptTimerOne() interrupt 3
{
    if (PWMOUT == 1)  //��ǰ���Ϊ�ߵ�ƽʱ��װ�ص͵�ƽֵ������͵�ƽ
    {
        TH1 = LowRH;
        TL1 = LowRL;
        PWMOUT = 0;
    }
    else              //��ǰ���Ϊ�͵�ƽʱ��װ�ظߵ�ƽֵ������ߵ�ƽ
    {
        TH1 = HighRH;
        TL1 = HighRL;
        PWMOUT = 1;
    }
}

void InterruptTimerTwo() interrupt 5
{
	uint8 code table[16] = {
	1, 5, 10,18, 23, 30, 41, 51, 60, 68, 75, 81, 86, 90, 93, 95
	};
	static bit dir = 0;
	static uint8 index = 0, flashCnt = 0;

	TF2 = 0;

	switch(mLedState){
		case LED_Breath:
			AdjustDutyCycle(table[index]);
			if(dir == 0)
			{
				index++;
				if(index >= 15)
				{
					dir = 1;
				}
			}else{
				index--;
				if(index == 0)
				{
					dir = 0;
				}
			}
			break;
		case LED_Flash:
			flashCnt++;
			if(flashCnt > 10){
				flashCnt = 0;
				PWMOUT = ~PWMOUT;
			}
			break;
		case LED_Off:
			if(!PWMOUT)
			{
				PWMOUT = 1;
			}
			break;
		default: break;
	}
}

void InitLED()
{
	ConfigPWM(100, 10);  //���ò�����PWM
   	ConfigTimerTwo();    //��T2��ʱ����ռ�ձ�
}

void LEDBreath()
{
	mLedState = LED_Breath;

	ET1 = 1;
	TR1 = 1; //������ʱ���ж�1��PWM���ȣ�

	ET2 = 1; //������ʱ���ж�2 ��PWM��ʾ�����
	TR2 = 1;
}

void LEDFlash()
{
	mLedState = LED_Flash;

	PWMOUT = 0;

	ET1 = 0;
	TR1 = 0; //�رն�ʱ���ж�1��Flash��ʹ�ø��ж�

	ET2 = 1;
	TR2 = 1; //������ʱ��2,500ms��˸
}

void LEDOff()
{
	mLedState = LED_Off;

	ET1 = 0;
	TR1 = 0; //�رն�ʱ���ж�1��Flash��ʹ�ø��ж�

	ET2 = 0;
	TR2 = 0; //�رն�ʱ��2,500ms��˸

	PWMOUT = 1;	//�ر�С��
}