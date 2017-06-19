/*
 * config.h
 * --------
 * func����������������Ҫ�õ��Ķ��塢���Ƶȵ�
 * edit by: DK
 * time:2017-05-08
 * edition 1
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <reg52.h>
#include <intrins.h>

//���ڽ��������͵�����
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
typedef signed char int8 ;
typedef signed int	int16 ;
typedef signed long	int32 ;

//����ϵͳʱ��
#define SYS_CLK (11059200/12)

//LCD12864���I/O��
#define LCD12864_DB P2

sbit LCD12864_EN = P0^7;
sbit LCD12864_RS = P0^5;
sbit LCD12864_RW = P0^6;

//���������I/O��
sbit BUZZ = P1^3;

//����������I/0��
sbit KeyCancel = P0^1;
sbit KeyAdj = P0^2;
sbit KeySet = P0^3;
sbit KeyPage = P0^4;

//DHT11��ʪ�ȴ�����
sbit DHT11_DATA = P1^4;

//DS1302ʱ������I/O�ڣ�SPI���ߣ�

sbit DS1302_CE = P1^2;
sbit DS1302_IO = P1^1;
sbit DS1302_CK = P1^0;

//��LED���������
sbit PWMOUT = P0^0;

#endif