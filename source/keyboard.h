 /*
 * keyboard.h
 * --------
 * func：关于按键(少量蜂鸣器)的接口函数以及全局变量
 */

 #ifndef _KEYBOARD_H
 #define _KEYBOARD_H

 #ifndef _KEYBOARD_C
 extern bit flag200ms;
 extern bit flag1s;
 extern bit flagBuzzOn;
 #endif

void ConfigTimerZero(uint8 ms);
void KeyDriver();

#endif