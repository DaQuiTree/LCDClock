/*
 * DHT11.h
 * --------
 * func：声明跟DHT11相关的接口函数
 */

 #ifndef _DHT11_H
 #define _DHT11_H

 #ifndef _DHT11_C
 //extern
 #endif

 bit DHT11GetData(uint8* humi, uint8* temp); 
 void ConfigureDHT11();

 #endif
