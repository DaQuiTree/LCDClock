/*
 * time.h
 * --------
 * func������time.c�з�װ�ĸ��⺯����Ϊmain.c�ṩ�ӿ�
 */

 #ifndef _TIME_H
 #define _TIME_H

 #ifndef _TIME_C
 //extern
 extern bit bClockOpen;
 extern uint8 cHour;
 extern uint8 cMin;
 #endif

 void ShowCurrentTime();
 void ShowTemp();
 void KeyAction(uint8 keyCode);

 #endif