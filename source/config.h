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

//LCD12864相关I/O口
#define LCD12864_DB P2

sbit LCD12864_EN = P0^7;
sbit LCD12864_RS = P0^5;
sbit LCD12864_RW = P0^6;

//蜂鸣器相关I/O口
sbit BUZZ = P1^3;

//矩阵键盘相关I/0口
sbit KeyCancel = P0^1;
sbit KeyAdj = P0^2;
sbit KeySet = P0^3;
sbit KeyPage = P0^4;

//DHT11温湿度传感器
sbit DHT11_DATA = P1^4;

//DS1302时钟所需I/O口（SPI总线）

sbit DS1302_CE = P1^2;
sbit DS1302_IO = P1^1;
sbit DS1302_CK = P1^0;

//和LED呼吸灯相关
sbit PWMOUT = P0^0;

#endif