/*
 * lcd12864.h
 * --------
 * func：声明跟lcd12864相关的接口函数
 */


#ifndef _LCD12864_H
#define _LCD12864_H

#ifndef _LCD12864_C
//extern 变量
#endif

void InitLCD12864();
void LCDShowStr(uint8 x, uint8 y, uint8* str, uint8 strlen);	//写入字符串
void LCDShowCN(uint8 x, uint8 y, uint16* str, uint8 strlen);	//写入中文
void LCDShowImage(uint8* image);//绘图
void LCDSetCursor(uint8 x, uint8 y);//设定光标
void LCDShowCursor();//显示光标
void LCDCancelCursor();//取消显示光标
void LCDClearAll();//清屏

#endif