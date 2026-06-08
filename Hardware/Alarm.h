#ifndef __ALARM_H
#define __ALARM_H

#include <stdint.h>

/****宏定义**************************/
#define ALARM_LIMIT					3

#define ALARM_INSERT_OK			0
#define ALARM_INSERT_ERROR	1
#define ALARM_DELETE_OK			0
#define ALARM_DELETE_ERROR	1
/**************************宏定义****/

/***模块数据结构定义*********************/
// 闹钟链表节点
typedef struct ALARM_NODE{
	uint32_t cnt;
	uint8_t sec;
	struct ALARM_NODE *next;
} *ALARM_NODE_TYPE;

typedef struct ALARM_LINK{
	ALARM_NODE_TYPE Alarm_node;
	uint8_t quantity;
	uint8_t capacity;
} *ALARM_LINK_TYPE;

typedef struct DATE_ELEMENT{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} DATE_ELEMENT_TYPE;

/*********************模块数据结构定义***/

/********全局变量定义*******************************/
extern ALARM_LINK_TYPE alarm_link;							// 闹钟链表头节点
extern DATE_ELEMENT_TYPE alarm_element_date[ALARM_LIMIT];		// 表达闹钟时间的数组
/*******************************全局变量定义********/

/*****功能性函数定义********************************/
void alarm_link_init(void);			// 初始化闹钟链表数据结构
uint8_t alarm_link_insert(uint32_t new_count, uint8_t sec);		// 插入新的闹钟
uint8_t alarm_link_delete(uint8_t i);			// 删除指定闹钟
void alarm_link_read(void);								// 读取所有闹钟
uint32_t alarm_link_headCount(void);			// 读取第一个闹钟的count值
uint32_t alarm_link_tailCount(void);			// 读取最后一个闹钟的count值
void alarm_link_cycle(void);							// 轮转链表
void alarm_link_rotationCycle(void);			// 逆轮转链表
uint8_t alarm_link_headSecond(void);			// 返回头任务的秒数
/*********************************功能性函数定义****/

#endif
