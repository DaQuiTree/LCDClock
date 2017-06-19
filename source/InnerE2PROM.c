   /* 
************************************************************************************ 
Fuction:STC89C52��Ƭ���ڲ�4K��EEPROM�������� 
Software Designer:Jason 
************************************************************************************ 
*/  
#include "config.h"  
#include "innerE2PROM.h"

/******************���������ֽ�******************/       
#define Read_COM    0x01                    //�ֽڶ���������     
#define Prog_COM    0x02                    //�ֽڱ����������     
#define Erase_COM    0x03                   //����������������      
#define En_Wait_TIME    0x81                //���õȴ�ʱ�� ,��ʹ��ISP/IAP        
#define Start_ADDRH 0x20                    //������ַ��λ      
#define Start_ADDRL 0x00                    //������ַ��λ 
 
/****************���⹦�ܼĴ�������****************/  
sfr ISP_DATA = 0xe2;   
sfr ISP_ADDRH = 0xe3;     
sfr ISP_ADDRL = 0xe4;   
sfr ISP_CMD = 0xe5;   
sfr ISP_TRIG = 0xe6;      
sfr ISP_CONTR = 0xe7;  

/*****************�ر�ISP&IAP����*****************/     
void ISP_IAP_disable()     
{     
    ISP_CONTR = 0x00;     
    ISP_CMD = 0x00;      
    ISP_TRIG = 0x00;     
}     

/*********************�ֽڶ�**********************/       
uint8 E2ByteRead(uint16 byte_addr)     
{     
    EA = 0;                                     //���ж�      
    ISP_CONTR = En_Wait_TIME;                   //����ISP&IAP�����͵ȴ�ʱ��      
    ISP_CMD = Read_COM;                         //���ֽڶ�������         
    ISP_ADDRH = (uint8)(byte_addr >> 8);          //�͵�ַ���ֽ�      
    ISP_ADDRL = (uint8)(byte_addr & 0x00ff);    //�͵�ַ���ֽ�          
    ISP_TRIG = 0x46;                            //�ʹ���������0x46     
    ISP_TRIG = 0xB9;                            //�ʹ���������0xB9     
    _nop_();     
    ISP_IAP_disable();                          //�ر�ISP&IAP����      
    EA = 1;                                     //���ж�      
    return (ISP_DATA);     
}       
/*********************�ֽڱ��*********************/     
void E2ByteProgram(uint16 byte_addr,uint8 isp_iap_data)     
{     
    EA = 0;                                     //���ж�      
    ISP_CONTR = En_Wait_TIME;                   //����ISP&IAP�����͵ȴ�ʱ��      
    ISP_CMD = Prog_COM;                         //���ֽڱ��������        
    ISP_ADDRH = (uint8)(byte_addr >> 8);          //�͵�ַ���ֽ�      
    ISP_ADDRL = (uint8)(byte_addr & 0x00ff);    //�͵�ַ���ֽ�          
    ISP_DATA = isp_iap_data;                    //�����ݽ�ISP_DATA      
    ISP_TRIG = 0x46;                            //�ʹ���������0x46     
    ISP_TRIG = 0xB9;                            //�ʹ���������0xB9     
    _nop_();     
    ISP_IAP_disable();                          //�ر�ISP&IAP����      
    EA = 1;                                     //���ж�         
}     
     
/*********************��������*********************/     
void E2SectorErase(uint16 sector_addr)     		
{     
    EA = 0;                                     //���ж�      
    ISP_CONTR = En_Wait_TIME;                   //����ISP&IAP;���͵ȴ�ʱ��      
    ISP_CMD = Erase_COM;                        //����������������        
    ISP_ADDRH = (uint8)(sector_addr >> 8);    //�͵�ַ���ֽ�      
    ISP_ADDRL = (uint8)(sector_addr & 0X00FF);  //�͵�ַ���ֽ�          
    ISP_TRIG = 0X46;                            //�ʹ���������0x46    
    ISP_TRIG = 0XB9;                            //�ʹ���������0xB9     
    _nop_();     
    ISP_IAP_disable();                          //�ر�ISP&IAP����      
    EA = 1;     
}    
