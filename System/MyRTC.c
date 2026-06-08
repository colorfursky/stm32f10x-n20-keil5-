#include "stm32f10x.h"                  // Device header
#include "MyRTC.h"
#include <time.h>

RTC_TIME MyRTC_Time = {2026, 4, 29, 2, 38, 00};

void MyRTC_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BKP_DR1, 0x0000);
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5){
		RCC_LSICmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(40000 - 1);
		RTC_WaitForLastTask();
		
		MyRTC_SetTime();
		RTC_WaitForLastTask();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}else{
		RCC_LSICmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
	
	RCC_RTCCLKCmd(ENABLE);
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	RTC_WaitForLastTask();
	
	RTC_SetAlarm(0xFFFFFFFF);
	RTC_WaitForLastTask();
	
	EXTI_InitTypeDef EXTI_InitStruct;
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStruct);
}

void MyRTC_SetTime(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year = MyRTC_Time.year - 1900;
	time_date.tm_mon = MyRTC_Time.month - 1;
	time_date.tm_mday = MyRTC_Time.day;
	time_date.tm_hour = MyRTC_Time.hour;
	time_date.tm_min = MyRTC_Time.minute;
	time_date.tm_sec = MyRTC_Time.second;
	
	time_cnt = mktime(&time_date);
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter();
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time.year = time_date.tm_year + 1900;
	MyRTC_Time.month = time_date.tm_mon + 1;
	MyRTC_Time.day = time_date.tm_mday;
	MyRTC_Time.hour = time_date.tm_hour;
	MyRTC_Time.minute = time_date.tm_min;
	MyRTC_Time.second = time_date.tm_sec;
}

void MyRTC_SetAlarm(uint32_t count){
	RTC_SetAlarm(count);
}
