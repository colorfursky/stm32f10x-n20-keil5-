#include "ds1302.h"
#include "sys.h"
#include "Delay.h"
 
/*****************?¨¬¡À|¦Ì?¡Á¨®******************
											STM32
 * ???t			:	DS1302¨º¡À?¨®?¡ê?¨¦¨º¦Ì?¨¦                     
 * ¡ã?¡À?			: V1.0
 * ¨¨??¨²			: 2025.6.2
 * MCU			:	STM32F103C8T6
 * ?¨®?¨²			:	??¡ä¨²??							
 * BILIBILI	:	?¨¬¡À|¦Ì?¡Á¨®
 * ¡Á¡Â??			:	¨¢¦Ì¡ã???_o-o-

**********************BEGIN***********************/ 
 
 
struct TIMEData TimeData;
struct TIMERAM TimeRAM;
u8 read_time[7];
 
void DS1302_GPIO_Init(void)//CE,SCLK???¨²3?¨º??¡¥
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS1302_SCLK_PIN; //CE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//¨ª?¨ª¨¬¨º?3?
	GPIO_Init(DS1302_SCLK_PORT, &GPIO_InitStructure);//3?¨º??¡¥
	GPIO_ResetBits(DS1302_SCLK_PORT,DS1302_SCLK_PIN); 
	 
	GPIO_InitStructure.GPIO_Pin = DS1302_CE_PIN; //SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//¨ª?¨ª¨¬¨º?3?
	GPIO_Init(DS1302_CE_PORT, &GPIO_InitStructure);//3?¨º??¡¥
	GPIO_ResetBits(DS1302_CE_PORT,DS1302_CE_PIN); 
}
 
void DS1302_DATAOUT_init()//???????¨°I/O???¨²?a¨º?3?¨¬?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = DS1302_DATA_PIN; // DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);//3?¨º??¡¥
	GPIO_ResetBits(DS1302_DATA_PORT,DS1302_DATA_PIN);
}

void DS1302_DATAINPUT_init()//???????¨°I/O???¨²?a¨º?¨¨?¨¬?
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS1302_DATA_PIN; //DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);//3?¨º??¡¥
}
 
 
void DS1302_write_onebyte(u8 data)//?¨°DS1302¡¤¡é?¨ª¨°?¡Á??¨²¨ºy?Y
{
		u8 count=0;
		SCLK_L;
		DS1302_DATAOUT_init();


	for(count=0;count<8;count++)
		{	SCLK_L;
			if(data&0x01)
			{DATA_H;}
			else{DATA_L;}//?¨¨¡Á?¡À?o?¨ºy?Y?¨´¡¤¡é?¨ª
			SCLK_H;//¨¤-??¨º¡À?¨®??¡ê?¡¤¡é?¨ª¨ºy?Y
			data>>=1;
		}
}
 
void DS1302_wirte_rig(u8 address,u8 data)//?¨°???¡§??¡ä??¡Â¦Ì??¡¤¡¤¡é?¨ª¨ºy?Y
{
	u8 temp1=address;
	u8 temp2=data;
	CE_L;SCLK_L;Delay_us(1);
	CE_H;Delay_us(3);
	DS1302_write_onebyte(temp1);
	DS1302_write_onebyte(temp2);
	CE_L;SCLK_L;Delay_us(3);
}
 
u8 DS1302_read_rig(u8 address)//¡ä¨®???¡§¦Ì??¡¤?¨¢¨¨?¨°?¡Á??¨²¨ºy?Y
{
	u8 temp3=address;
	u8 count=0;
	u8 return_data=0x00;
	CE_L;SCLK_L;Delay_us(3);
	CE_H;Delay_us(3);
	DS1302_write_onebyte(temp3);
	DS1302_DATAINPUT_init();//????I/O?¨²?a¨º?¨¨?
	Delay_us(3);
	for(count=0;count<8;count++)
		{
		Delay_us(3);//¨º1¦Ì???3?D?¨°???¨º¡À??
		return_data>>=1;
		SCLK_H;Delay_us(5);//¨º1??¦Ì???3?D?¨°???¨º¡À??
		SCLK_L;Delay_us(30);//?¨®¨º¡À14uso¨®?¨´¨¨£¤?¨¢¨¨?¦Ì??1¡ê??¨¹?¨®¡Á?¨¨¡¤
		if(GPIO_ReadInputDataBit(DS1302_DATA_PORT,DS1302_DATA_PIN))
		{return_data=return_data|0x80;}
		
		}
	Delay_us(2);
	CE_L;DATA_L;
	return return_data;
}
 
void DS1302_Init(void)
{
	DS1302_wirte_rig(0x8e,0x00);//1?¡À?D¡ä¡À¡ê?¡è
	DS1302_wirte_rig(0x80,0x00);//seconds00??
	DS1302_wirte_rig(0x82,0x31);//minutes45¡¤?
	DS1302_wirte_rig(0x84,0x13);//hours22¨º¡À
	DS1302_wirte_rig(0x86,0x15);//date5¨¨?
	DS1302_wirte_rig(0x88,0x05);//months12??
	DS1302_wirte_rig(0x8a,0x05);//daysD??¨²??
	DS1302_wirte_rig(0x8c,0x26);//year2025?¨º
	DS1302_wirte_rig(0x8e,0x80);//?a??D¡ä¡À¡ê?¡è
}
 
void DS1302_read_time(void)
{
	read_time[0]=DS1302_read_rig(0x81);//?¨¢??
	read_time[1]=DS1302_read_rig(0x83);//?¨¢¡¤?
	read_time[2]=DS1302_read_rig(0x85);//?¨¢¨º¡À
	read_time[3]=DS1302_read_rig(0x87);//?¨¢¨¨?
	read_time[4]=DS1302_read_rig(0x89);//?¨¢??
	read_time[5]=DS1302_read_rig(0x8B);//?¨¢D??¨²
	read_time[6]=DS1302_read_rig(0x8D);//?¨¢?¨º
}
 
void DS1302_read_realTime(void)
{
	DS1302_read_time();  //BCD??¡Áa???a10????
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;	
}


void DS1302_wirteRAM(void)
{
	DS1302_wirte_rig(0x8e,0x00);//1?¡À?D¡ä¡À¡ê?¡è
	DS1302_wirte_rig(0xC0,TimeRAM.hour_kai);//?a¨º¡À
	DS1302_wirte_rig(0xC2,TimeRAM.minute_kai);//?a¡¤?
	DS1302_wirte_rig(0xC4,TimeRAM.hour_guan);//1?¨º¡À
	DS1302_wirte_rig(0xC6,TimeRAM.minute_guan);//1?¡¤?
	DS1302_wirte_rig(0xC8,TimeRAM.kai);//1?¡¤?
	DS1302_wirte_rig(0xCA,TimeRAM.guan);//1?¡¤?
	DS1302_wirte_rig(0x8e,0x80);//1?¡À?D¡ä¡À¡ê?¡è
}
void DS1302_readRAM(void)
{
	TimeRAM.hour_kai=DS1302_read_rig(0xC1);//?¨¢??
	TimeRAM.minute_kai=DS1302_read_rig(0xC3);//?¨¢¡¤?
	TimeRAM.hour_guan=DS1302_read_rig(0xC5);//?¨¢¨º¡À
	TimeRAM.minute_guan=DS1302_read_rig(0xC7);//?¨¢¨¨?	
	TimeRAM.kai=DS1302_read_rig(0xC9);//?¨¢¨¨?
	TimeRAM.guan=DS1302_read_rig(0xCB);//?¨¢¨¨?
}
 




