#define _PWMLED_C

#include "config.h"
#include "pwmled.h"

ledState mLedState = LED_Breath;

uint8 HighRH = 0;  //高电平重载值的高字节
uint8 HighRL = 0;  //高电平重载值的低字节
uint8 LowRH  = 0;  //低电平重载值的高字节
uint8 LowRL  = 0;  //低电平重载值的低字节

uint32 PeriodCnt = 0;

void ConfigTimerTwo() //定时器二 50ms
{

	RCAP2H = (uint8)((65535 - SYS_CLK/1000*50 + 12) >> 8); //设置定时器二为显示器件定时器
	RCAP2L = (uint8)(65535 - SYS_CLK/1000*50 + 12);

	T2CON = 0x00;

	TH2 = RCAP2H;
	TL2 = RCAP2L;

	TR2 = 0; 	// T2中断暂时关闭，等待调用
	//PT2 = 1;//设置LED显示处于最高中断优先级
	ET2 = 0;
}

void ConfigPWM(uint16 fr, uint8 dc)
{
    uint16 high, low;
    
    PeriodCnt = SYS_CLK / fr; //计算一个周期所需的计数值
    high = (PeriodCnt*dc) / 100;    //计算高电平所需的计数值
    low  = PeriodCnt - high;        //计算低电平所需的计数值
    high = 65536 - high + 12;       //计算高电平的定时器重载值并补偿中断延时
    low  = 65536 - low  + 12;       //计算低电平的定时器重载值并补偿中断延时
    HighRH = (uint8)(high>>8); //高电平重载值拆分为高低字节
    HighRL = (uint8)high;
    LowRH  = (uint8)(low>>8);  //低电平重载值拆分为高低字节
    LowRL  = (uint8)low;
    TMOD &= 0x0F;   //清零T1的控制位
    TMOD |= 0x10;   //配置T1为模式1
    TH1 = HighRH;   //加载T1重载值
    TL1 = HighRL;
    ET1 = 0;        //关闭T1中断，等待调用
    TR1 = 0;        //关闭T1
    PWMOUT = 1;     //输出高电平
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
    if (PWMOUT == 1)  //当前输出为高电平时，装载低电平值并输出低电平
    {
        TH1 = LowRH;
        TL1 = LowRL;
        PWMOUT = 0;
    }
    else              //当前输出为低电平时，装载高电平值并输出高电平
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
	ConfigPWM(100, 10);  //配置并启动PWM
   	ConfigTimerTwo();    //用T2定时调整占空比
}

void LEDBreath()
{
	mLedState = LED_Breath;

	ET1 = 1;
	TR1 = 1; //开启定时器中断1（PWM亮度）

	ET2 = 1; //开启定时器中断2 （PWM显示间隔）
	TR2 = 1;
}

void LEDFlash()
{
	mLedState = LED_Flash;

	PWMOUT = 0;

	ET1 = 0;
	TR1 = 0; //关闭定时器中断1，Flash不使用该中断

	ET2 = 1;
	TR2 = 1; //开启定时器2,500ms闪烁
}

void LEDOff()
{
	mLedState = LED_Off;

	ET1 = 0;
	TR1 = 0; //关闭定时器中断1，Flash不使用该中断

	ET2 = 0;
	TR2 = 0; //关闭定时器2,500ms闪烁

	PWMOUT = 1;	//关闭小灯
}