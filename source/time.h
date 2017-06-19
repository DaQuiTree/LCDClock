/*
 * time.h
 * --------
 * func：声明time.c中封装的各库函数，为main.c提供接口
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