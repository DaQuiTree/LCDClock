#include "config.h"
#include "dht11.h"

#define _DHT11_C

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
	uint8 i,j;
	uint8 mdat[5];

	EA = 0;
	ack = DHT11GetACK();
	if(!ack)return ack;  //读数失败
	cntThr = 0;
	while(!DHT11_DATA && cntThr++ < NUMBER);	  //等待DHT11拉高电平
	if(cntThr >= NUMBER){
		DHT11_DATA = 1;
		EA = 1;
		return 0;
	}
	DelayX10us(8);	

	for(i = 0; i < 5; i++){
		for(j = 0; j < 8; j++){
			cntThr = 0;
			while(!DHT11_DATA && cntThr++ < NUMBER);	//等待高电平到来
			if(cntThr >= NUMBER){
				DHT11_DATA = 1;
				EA = 1;
				return 0;
			}		  
			DelayX10us(3);
			mdat[i] <<= 1;
			if(DHT11_DATA){	//若为高电平
				mdat[i] |= 0x01;
				DelayX10us(3);//将高电平延时过去
			}
		}	
	}

	*humi = mdat[0];
	*temp = mdat[2];
	EA = 1;
	return 1;	
}