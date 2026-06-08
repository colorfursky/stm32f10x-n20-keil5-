#ifndef __MYRTC_H
#define __MYRTC_H

#include <stdint.h>
#include <time.h>

typedef struct{
	int16_t year;
	int8_t month;
	int8_t day;
	int8_t hour;
	int8_t minute;
	int8_t second;
}RTC_TIME;

extern RTC_TIME MyRTC_Time;
//extern time_t RTC_Curtime;
//extern uint32_t Alarm_count;

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
void MyRTC_SetAlarm(uint32_t count);

#endif
