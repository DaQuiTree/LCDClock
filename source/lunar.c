#define _LUNAR_C
#include "config.h"
#include "lcd12864.h"
 
uint32 code LunarCalendarTable[99] =  
{    
    0x4D4AB8,0x0D4A4C,0x0DA541,0x25AAB6,0x056A49,0x7AADBD,0x025D52,0x092D47,0x5C95BA,0x0A954E,/*2001-2010*/  
    0x0B4A43,0x4B5537,0x0AD54A,0x955ABF,0x04BA53,0x0A5B48,0x652BBC,0x052B50,0x0A9345,0x474AB9,/*2011-2020*/  
    0x06AA4C,0x0AD541,0x24DAB6,0x04B64A,0x69573D,0x0A4E51,0x0D2646,0x5E933A,0x0D534D,0x05AA43,/*2021-2030*/  
    0x36B537,0x096D4B,0xB4AEBF,0x04AD53,0x0A4D48,0x6D25BC,0x0D254F,0x0D5244,0x5DAA38,0x0B5A4C,/*2031-2040*/  
    0x056D41,0x24ADB6,0x049B4A,0x7A4BBE,0x0A4B51,0x0AA546,0x5B52BA,0x06D24E,0x0ADA42,0x355B37,/*2041-2050*/  
    0x09374B,0x8497C1,0x049753,0x064B48,0x66A53C,0x0EA54F,0x06B244,0x4AB638,0x0AAE4C,0x092E42,/*2051-2060*/  
    0x3C9735,0x0C9649,0x7D4ABD,0x0D4A51,0x0DA545,0x55AABA,0x056A4E,0x0A6D43,0x452EB7,0x052D4B,/*2061-2070*/  
    0x8A95BF,0x0A9553,0x0B4A47,0x6B553B,0x0AD54F,0x055A45,0x4A5D38,0x0A5B4C,0x052B42,0x3A93B6,/*2071-2080*/  
    0x069349,0x7729BD,0x06AA51,0x0AD546,0x54DABA,0x04B64E,0x0A5743,0x452738,0x0D264A,0x8E933E,/*2081-2090*/  
    0x0D5252,0x0DAA47,0x66B53B,0x056D4F,0x04AE45,0x4A4EB9,0x0A4D4C,0x0D1541,0x2D92B5          /*2091-2099*/   
}; 
 
uint32 code MonthAdd[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
 
uint16 code ChDay[31][2] = {
{0x0000,0x0000},{0xB3F5,0xD2BB},{0xB3F5,0xB6FE},{0xB3F5,0xC8FD},{0xB3F5,0xCBC4},{0xB3F5,0xCEE5},
				{0xB3F5,0xC1F9},{0xB3F5,0xC6DF},{0xB3F5,0xB0CB},{0xB3F5,0xBEC5},{0xB3F5,0xCAAE},//初一~初十
				{0xCAAE,0xD2BB},{0xCAAE,0xB6FE},{0xCAAE,0xC8FD},{0xCAAE,0xCBC4},{0xCAAE,0xCEE5},
				{0xCAAE,0xC1F9},{0xCAAE,0xC6DF},{0xCAAE,0xB0CB},{0xCAAE,0xBEC5},{0xB6FE,0xCAAE},//十一~二十
				{0xD8A5,0xD2BB},{0xD8A5,0xB6FE},{0xD8A5,0xC8FD},{0xD8A5,0xCBC4},{0xD8A5,0xCEE5},
				{0xD8A5,0xC1F9},{0xD8A5,0xC6DF},{0xD8A5,0xB0CB},{0xD8A5,0xBEC5},{0xC8FD,0xCAAE},//廿一~三十
	};
	                          
uint16 code ChMonth[] = {
	0x0000,0xD5FD,0xB6FE,0xC8FD,0xCBC4,0xCEE5,
	0xC1F9,0xC6DF,0xB0CB,0xBEC5,0xCAAE,0xB6AC,0xC0B0
};//正月到腊月

uint16  code ChAnimal[] = {
	0xCAF3,0xC5A3,0xBBA2,0xCDC3,0xC1FA,0xC9DF,
	0xC2ED,0xD1F2,0xBAEF,0xBCA6,0xB9B7,0xD6ED
};		//{"鼠","牛","虎","兔","龙","蛇","马","羊","猴","鸡","狗","猪"}

uint32 LunarCalendarDay;
bit animalNewYear; 

bit LunarCalendar(uint8 year,uint8 month,uint8 day)  
{  
    uint16 Spring_NY,Sun_NY;  
    uint8 index,flag,StaticDayCount;  
    //Spring_NY 记录春节离当年元旦的天数。  
    //Sun_NY 记录阳历日离当年元旦的天数。  
    if ( ((LunarCalendarTable[year-1] & 0x0060) >> 5) == 1)  
        Spring_NY = (LunarCalendarTable[year-1] & 0x001F) - 1;  
    else  
        Spring_NY = (LunarCalendarTable[year-1] & 0x001F) - 1 + 31;  
    Sun_NY = MonthAdd[month-1] + day - 1;  
    if ( (!(year % 4)) && (month > 2))  
        Sun_NY++;  
    //StaticDayCount记录大小月的天数 29 或30  
    //index 记录从哪个月开始来计算。  
    //flag 是用来对闰月的特殊处理。  
    //判断阳历日在春节前还是春节后  
    if (Sun_NY >= Spring_NY)//阳历日在春节后（含春节那天）  
    {  
		animalNewYear = 1;
        Sun_NY -= Spring_NY;  
        month = 1;  
        index = 1;  
        flag = 0;  
        if ( ( LunarCalendarTable[year - 1] & (0x80000 >> (index-1)) ) ==0)  
            StaticDayCount = 29;  
        else  
            StaticDayCount = 30;  
        while (Sun_NY >= StaticDayCount)  
        {  
            Sun_NY -= StaticDayCount;  
            index++;  
            if (month == ((LunarCalendarTable[year - 1] & 0xF00000) >> 20) )  
            {  
                flag = ~flag;  
                if (flag == 0)  
                    month++;  
            }  
            else  
                month++;  
            if ( ( LunarCalendarTable[year - 1] & (0x80000 >> (index-1)) ) ==0)  
                StaticDayCount=29;  
            else  
                StaticDayCount=30;  
        }  
        day = Sun_NY + 1;  
    }  
    else //阳历日在春节前  
    {  
		animalNewYear = 0;
        Spring_NY -= Sun_NY;  
        year--;  
        month = 12;  
        if ( ((LunarCalendarTable[year - 1] & 0xF00000) >> 20) == 0)  
            index = 12;  
        else  
            index = 13;  
        flag = 0;  
        if ( ( LunarCalendarTable[year - 1] & (0x80000 >> (index-1)) ) ==0)  
            StaticDayCount = 29;  
        else  
            StaticDayCount = 30;  
        while (Spring_NY > StaticDayCount)  
        {  
            Spring_NY -= StaticDayCount;  
            index--;  
            if (flag == 0)  
                month--;  
            if (month == ((LunarCalendarTable[year - 1] & 0xF00000) >> 20))  
                flag = ~flag;  
            if ( ( LunarCalendarTable[year - 1] & (0x80000 >> (index-1)) ) == 0)  
                StaticDayCount = 29;  
            else  
                StaticDayCount = 30;  
        }  
        day = StaticDayCount - Spring_NY + 1;  
    }  
    LunarCalendarDay |= (uint32)day;  
    LunarCalendarDay |= (uint32)(month << 6);
    if (month == ((LunarCalendarTable[year - 1] & 0xF00000) >> 20))  
        return 1;  
    else  
        return 0;  
}

uint8 GetLunarDate(uint8 year, uint8 month, uint8 day, uint16 *lunarDate)  
{    
	uint8 i = 0;
	uint8 aniPos;  

	if (LunarCalendar(year,month,day))  
	{  
		lunarDate[i++] = 0xC8F2;  
	   	lunarDate[i++] = ChMonth[(LunarCalendarDay & 0x3C0) >> 6];  
	}else{  
	   lunarDate[i++] = ChMonth[(LunarCalendarDay & 0x3C0) >> 6]; 
	} 
	lunarDate[i++] = 0xD4C2;
	lunarDate[i++] = ChDay[LunarCalendarDay & 0x3F][0];
	lunarDate[i++] = ChDay[LunarCalendarDay & 0x3F][1];
	lunarDate[i++] = 0xA1A0; //空格
	aniPos = (year + 2000 - 1900)%12;
	if(animalNewYear)
	{
		lunarDate[i++] = ChAnimal[aniPos]; //过了春节属相计算
	}else{
		if(aniPos == 0) 
		{
			aniPos = 11;
		}else{
			aniPos--;
		}
		lunarDate[i++] = ChAnimal[aniPos]; //未过春节属相计算
	}
	
	return i;
} 