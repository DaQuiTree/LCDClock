/*
 * DHT11.h
 * --------
 * func��������DHT11��صĽӿں���
 */

 #ifndef _DHT11_H
 #define _DHT11_H

 #ifndef _DHT11_C
 //extern
 #endif

 bit DHT11GetACK();
 bit DHT11GetData(uint8* humi, uint8* temp); 
 void ConfigureDHT11();

 #endif
