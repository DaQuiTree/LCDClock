/*
 * main.h
 * --------
 * func������ȫ�ֵ�һЩ����
 * edit by: DK
 * time:2017-05-08
 * edition 1
 */
							  
#ifndef _MAIN_H
#define _MAIN_H
 
 typedef enum{
 	ReadDate,ReadInfo,SetDate,SetClock
 } SystemMode;
 
 #ifndef _MAIN_C
 extern SystemMode mMode;
 #endif 

 #endif