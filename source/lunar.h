/*
 * lunar.h
 * --------
 * func：声明跟农历转换相关的接口函数
 */

 #ifndef _LUNAR_H
 #define _LUNAR_H

 uint8 GetLunarDate(uint8 year, uint8 month, uint8 day, uint16 *lunarDate);

 #endif