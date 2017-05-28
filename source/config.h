/*
 * config.h
 * --------
 * func：声明整个工程需要用到的定义、代称等等
 * edit by: DK
 * time:2017-05-08
 * edition 1
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <reg52.h>
#include <intrins.h>

//简化内建数据类型的名称
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
typedef signed char int8 ;
typedef signed int	int16 ;
typedef signed long	int32 ;

//定义系统时钟
#define SYS_CLK (11059200/12)

//LED显示
#define LED_DB P0

//LCD1602相关I/O口
#define LCD1602_DB P0

sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_EN = P1^5;

//LCD12864相关I/O口
#define LCD12864_DB P0

sbit LCD12864_EN = P1^5;
sbit LCD12864_RS = P1^0;
sbit LCD12864_RW = P1^1;

//74HC138定义(选通显示器件相关I/O口)
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit ENLCD = P1^5;

//蜂鸣器相关I/O口
sbit BUZZ = P1^6;

//矩阵键盘相关I/0口
sbit KeyIn4 = P2^7;
sbit KeyIn3 = P2^6;
sbit KeyIn2 = P2^5;
sbit KeyIn1 = P2^4;
sbit KeyOut1 = P2^3;
sbit KeyOut2 = P2^2;
sbit KeyOut3 = P2^1;
sbit KeyOut4 = P2^0;

//DS18B20温度传感器相关I/O口
sbit IO_18B20 = P3^2;

//DHT11温湿度传感器
sbit DHT11_DATA = P3^2;

//红外模块相关I/O口
sbit IR_STATE = P3^3;

//DS1302时钟所需I/O口（SPI总线）

sbit DS1302_CE = P1^7;
sbit DS1302_IO = P3^4;
sbit DS1302_CK = P3^5;

#endif