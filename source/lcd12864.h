/*
 * lcd12864.h
 * --------
 * func：声明跟lcd12864相关的接口函数
 */


#ifndef _LCD12864_H
#define _LCD12864_H

#ifndef _LCD12864_C //extern 变量
extern uint8 code clockImage[32];
extern uint8 code moonImage[32];
extern uint8 code sunImage[32];
extern uint8 code SaveButton[32];
extern uint8 code OpenVolume[32];
extern uint8 code StopVolume[32];
#endif

void InitLCD12864();//初始化液晶
void LCDClearAll();//清屏

/*字符显示*/
void LCDShowStr(uint8 x, uint8 y, uint8* str, uint8 strlen);	//写入字符串
void LCDShowCN(uint8 x, uint8 y, uint16* str, uint8 strlen);	//写入中文
void LCDClearArea(uint8 x, uint8 y);

/*光标设置*/
void LCDSetCursor(uint8 x, uint8 y);//设定光标
void LCDShowCursor();//显示光标
void LCDCancelCursor();//取消显示光标

/* 绘图函数 */
void LCDInitImage(); //初始化图像RAM（置0）
void LCDClearImage(uint8 x, uint8 y);	 //清空16*16区域
void LCDDrawArea(uint8 x, uint8 y, uint8* image);	//画16*16图像
void LCDShowImage(uint8* image);//绘全屏图

#endif