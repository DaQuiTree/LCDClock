/*
 * innerE2PROM.h
 * -------------
 * 说明：该头文件提供关于STC89C52RC系列单片机读写
 * 	     内部E2PROM的接口函数
 */

 #ifndef _INNERE2PROM_H
 #define _INNERE2PROM_H  

 void E2SectorErase(uint16 sector_addr);     		
 void E2ByteProgram(uint16 byte_addr,uint8 isp_iap_data);
 uint8 E2ByteRead(uint16 byte_addr);

 #endif