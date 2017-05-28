/*
 * lunar.h
 * --------
 * func：声明跟农历转换相关的接口函数
 */

 #ifndef _DHT11_H
 #define _DHT11_H

 uint8 GetLunarDate(uint32 year, uint32 month, uint32 day, uint16 *lunarDate);

 #endif