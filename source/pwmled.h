/*
 * PWMLED.h
 * --------
 * func: ����PWM�����ƵĽӿ�
 */

 #ifndef _PWMLED_H
 #define _PWMLED_H

 typedef enum { 
 	LED_Breath,LED_Flash,LED_Off
 }ledState;

 #ifndef _PWMLED_C
 //extern
 #endif

void InitLED();
void LEDBreath();
void LEDFlash();
void LEDOff();

 #endif