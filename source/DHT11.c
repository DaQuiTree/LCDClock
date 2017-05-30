#define _DHT11_C
#include "config.h"
#include "dht11.h"

#define NUMBER 20
uint8 cntThr = 0;

void DelayX10us(uint16 x)
{
	
	do{
		_nop_();
		_nop_();
	}while(--x);	
}

bit DHT11GetACK()
{
	bit ack;

	DHT11_DATA = 0;
	DelayX10us(2000);//延时20ms
	DHT11_DATA = 1;								   
	DelayX10us(4);
	ack = DHT11_DATA;
	
	return ~ack;	
}


bit DHT11GetData(uint8* humi, uint8* temp) 
{
	bit ack;
	uint8 cnt = 32;
	uint32 mdat = 0;

	EA = 0;
	ack = DHT11GetACK();
	if(!ack)return ack;  //读数失败
	cntThr = 0;
	while(!DHT11_DATA && cntThr++ < NUMBER);	  //等待DHT11拉高电平
	if(cntThr++ >= NUMBER){
		DHT11_DATA = 1;
		EA = 1;
		return 0;
	}
	DelayX10us(8);	

	while(cnt--){
		cntThr = 0;
		while(!DHT11_DATA && cntThr++ < NUMBER);	//等待高电平到来
		if(cntThr++ >= NUMBER){
			DHT11_DATA = 1;
			EA = 1;
			return 0;
		}		  
		TL1 = 0;
		TR1 = 1;
		while(DHT11_DATA);	//等待高电平结束
		TR1 = 0;			//结束计时
		if(TL1 < 30){  //低电平
			mdat <<= 1; 
		}else{			//高电平
			mdat <<= 1;
			mdat |= 0x1;
		}		
	}

	*humi = (uint8)(mdat >> 24);
	*temp = (uint8)(mdat >> 8);
	EA = 1;
	return 1;	
}

void ConfigureDHT11()
{
	TMOD &= 0x0F;
	TMOD |= 0x10;

	ET1 = 0;
}