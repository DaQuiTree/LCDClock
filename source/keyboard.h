 /*
 * keyboard.h
 * --------
 * func�����ڰ����Ľӿں����Լ�ȫ�ֱ���
 */

 #ifndef _KEYBOARD_H
 #define _KEYBOARD_H

 #ifndef _KEYBOARD_C
 extern bit flag200ms;
 extern bit flag1s;
 #endif

void ConfigTimerZero(uint8 ms);
void KeyDriver();

#endif