#ifndef __MYKEY_H
#define __MYKEY_H

#include "stm32f10x.h"                  // Device header

#define DELAY 5							// 消抖延时
#define QUEUE_CAPACITY 10		// 队列容量

/*---------按键接口定义----------*/
#define KEY_LEFT 				GPIO_Pin_10
#define KEY_LEFT_TYPE		GPIOA
#define KEY_MIDDLE			GPIO_Pin_11
#define KYE_MIDDLE_TYPE	GPIOA
#define KEY_RIGHT				GPIO_Pin_12
#define KEY_RIGHT_TYPE	GPIOA
/*---------可添加接口------------*/

struct KEY_INFO{
	uint8_t Num_name;
	GPIO_TypeDef * Type;	// GPIOX
	uint16_t Pin;			// 链接引脚
	uint8_t Count;		// 持续时长
	uint8_t State;		// 状态机 0待机 1按下 2弹起
};

struct KEY_QUEUE{
	uint8_t p, q;					// 双指针，前指针和后指针
	uint8_t info[QUEUE_CAPACITY];			// 队列容量
};


void MyKey_Init(void);			// 按键初始化
uint8_t MyKey_Scan(void);		// 按键扫描

#endif
