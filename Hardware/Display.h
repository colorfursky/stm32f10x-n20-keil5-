#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "Alarm.h"

/****宏定义**************************/
#define DISPLAY_MAX_CAPACITY 3

#define DISPLAY_PUSH_OK			0
#define DISPLAY_PUSH_ERROR	1

#define DISPLAY_POP_OK			0
#define DISPLAY_POP_ERROR		1

#define KEY_LEFT_NUMBER			1
#define KEY_MIDDLE_NUMBER		2
#define KEY_RIGHT_NUMBER		3
/**************宏定义****************/

/***模块数据结构定义*********************/
// 界面状态枚举定义
typedef enum DISPLAY{
	MAIN_MENUE = 1,
	SET_TIME_1,
	SET_TIME_2,
	CHECK_TIME_1,
	CHECK_TIME_2,
	DELET_TIME_1,
	DELET_TIME_2
} DISPLAY_TYPE;

// 显示相关信息存储
typedef struct DISPLAY_INFO{
	DISPLAY_TYPE status;
	uint8_t select;
	struct DISPLAY_INFO *last;
} *DISPLAY_INFO_TYPE;

// 显示人机交互顺序相关存储
typedef struct DISPLAY_INFO_STACK{
	DISPLAY_INFO_TYPE stack;
	uint8_t quantity;
	uint8_t capacity;
} *DISPLAY_INFO_STACK;

/***********************模块数据结构定义***/

/********全局变量定义*******************************/
extern DISPLAY_INFO_STACK display_stack;		// 显示栈头节点
extern DATE_ELEMENT_TYPE temp_time;
extern uint8_t set_time_state;
/********************************全局变量定义*******/

/*****功能性函数定义********************************/
void display_stack_init(void);						// 初始化显示人机交互栈
uint8_t display_stack_push(DISPLAY_TYPE enter_status); 		// 显示栈压栈操作
uint8_t display_stack_pop(void);					// 显示栈弹出栈顶
DISPLAY_TYPE display_GetStatus(void);			// 获取当前界面状态
uint8_t display_GetSelect(void);					// 获取当前界面选择
void display_keyEvent(uint8_t event);			// 处理目前的界面状态下的按键事件
/********************************功能性函数定义*****/

#endif
