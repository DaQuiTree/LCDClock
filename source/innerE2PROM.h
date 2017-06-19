/*
 * innerE2PROM.h
 * -------------
 * ˵������ͷ�ļ��ṩ����STC89C52RCϵ�е�Ƭ����д
 * 	     �ڲ�E2PROM�Ľӿں���
 */

 #ifndef _INNERE2PROM_H
 #define _INNERE2PROM_H  

 void E2SectorErase(uint16 sector_addr);     		
 void E2ByteProgram(uint16 byte_addr,uint8 isp_iap_data);
 uint8 E2ByteRead(uint16 byte_addr);

 #endif