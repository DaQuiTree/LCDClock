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
	DelayX10us(2000);//��ʱ20ms
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
	if(!ack)return ack;  //����ʧ��
	cntThr = 0;
	while(!DHT11_DATA && cntThr++ < NUMBER);	  //�ȴ�DHT11���ߵ�ƽ
	if(cntThr++ >= NUMBER){
		DHT11_DATA = 1;
		EA = 1;
		return 0;
	}
	DelayX10us(8);	

	while(cnt--){
		cntThr = 0;
		while(!DHT11_DATA && cntThr++ < NUMBER);	//�ȴ��ߵ�ƽ����
		if(cntThr++ >= NUMBER){
			DHT11_DATA = 1;
			EA = 1;
			return 0;
		}		  
		TL1 = 0;
		TR1 = 1;
		while(DHT11_DATA);	//�ȴ��ߵ�ƽ����
		TR1 = 0;			//������ʱ
		if(TL1 < 30){  //�͵�ƽ
			mdat <<= 1; 
		}else{			//�ߵ�ƽ
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