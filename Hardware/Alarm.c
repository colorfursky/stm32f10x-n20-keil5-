#include "stm32f10x.h"                  // Device header
#include "Alarm.h"
#include "MyRTC.h"

#include <stdlib.h>

ALARM_LINK_TYPE alarm_link;
DATE_ELEMENT_TYPE alarm_element_date[ALARM_LIMIT];

/**
	*函数：将存放闹钟数据的链表初始化
	*参数：无
	*返回值：无
	*说明：初始化会给链表头节点分配空间，默认实际没有存储闹钟
*/
void alarm_link_init(void){
	// 初始化闹钟链表数据结构
	alarm_link = (ALARM_LINK_TYPE) malloc(sizeof(struct ALARM_LINK));
	if(alarm_link == NULL){
		while(1);
	}
	alarm_link->quantity = 0;								// 闹钟数量初始化
	alarm_link->capacity = ALARM_LIMIT;			// 闹钟容量初始化
	alarm_link->Alarm_node = (ALARM_NODE_TYPE) malloc(sizeof(struct ALARM_NODE));
	if(alarm_link->Alarm_node == NULL){
		free(alarm_link);
		while(1);
	}
	
// 初始化闹钟链表头节点
	alarm_link->Alarm_node->cnt = 0xFFFFFFFF;
	alarm_link->Alarm_node->next = NULL;
}

/**
	*函数：将新的闹钟按从小到大的顺序插入给链表
	*参数：闹钟触发时刻
	*参数：闹钟执行给定的时间
	*返回值：返回0或1。即"Alarm.h"宏定义部分包含的 ALARM_INSERT_OK 和 ALARM_INSERT_ERROR
	*说明：如果插入的位置的第一个，那么会改变实时时钟的闹钟count值。
*/
uint8_t alarm_link_insert(uint32_t new_count, uint8_t sec){
// 不允许闹钟满的情况下新插入
	if(alarm_link->quantity == alarm_link->capacity){
		return ALARM_INSERT_ERROR;
	}
// 不允许sec不合法
	if(sec <= 0 || sec > 60){
		return ALARM_INSERT_ERROR;
	}
// 定义新节点分配新数据
	ALARM_NODE_TYPE alarm_new_node = (ALARM_NODE_TYPE) malloc(sizeof(struct ALARM_NODE));
// 检查是否分配成功
	if(alarm_new_node == NULL){
		return ALARM_INSERT_ERROR;
	}
// 遍历搜索需要插入的位置
	ALARM_NODE_TYPE alarm_index_node = alarm_link->Alarm_node;		// 定义索引
	while(alarm_index_node->next != NULL){
		if(new_count < alarm_index_node->next->cnt)
			break;
		if(new_count == alarm_index_node->next->cnt){		// 如果闹钟重合就覆盖
			alarm_index_node->next->sec = sec;
			return ALARM_INSERT_OK;
		}
		alarm_index_node = alarm_index_node->next;		// 移动下一个节点
	}														// 如果遇到尽头或遇到比新的时刻还要大的就停止移动
// 对链表进行插入操作
	alarm_new_node->next = alarm_index_node->next;		// 让新节点指向后节点
	alarm_index_node->next = alarm_new_node;					// 让前节点指向新节点

// 更新新节点数据
	alarm_new_node->cnt = new_count;
	alarm_new_node->sec = sec;
// 更新链表状态
	alarm_link->quantity++;
// 额外判断是否插入影响到目前闹钟触发
	if(alarm_index_node->cnt == 0xFFFFFFFF){
		MyRTC_SetAlarm(alarm_link->Alarm_node->next->cnt);
	}
	return ALARM_INSERT_OK;
}

/**
	*函数：删除指定顺序的闹钟
	*参数：闹钟序号
	*返回值：返回0或1。即"Alarm.h"宏定义部分包含的 ALARM_DELETE_OK 和 ALARM_DELETE_ERROR
	*说明：
*/
uint8_t alarm_link_delete(uint8_t i){
// 不允许非法参数
	if(i <= 0 || i > alarm_link->quantity)
		return ALARM_DELETE_ERROR;
// 定义链表索引找到目标节点
	ALARM_NODE_TYPE alarm_index_node = alarm_link->Alarm_node;
	ALARM_NODE_TYPE p;		// 定义标记节点
	while(--i){
		alarm_index_node = alarm_index_node->next;
	}   // 寻找目标节点
// 链表删除操作
	p = alarm_index_node->next;
	alarm_index_node->next = alarm_index_node->next->next;
	free(p);
// 更新链表状态
	alarm_link->quantity--;
	RTC_SetAlarm(alarm_link_headCount());
	return ALARM_DELETE_OK;
}

/**
	*函数：遍历闹钟链表返回对应日期
	*参数：无
	*返回值：无
	*说明：返回当前闹钟任务从先到后的时间
*/
void alarm_link_read(void){
// 定义数组索引和链表索引
	uint8_t i = 0;
	ALARM_NODE_TYPE alarm_index_node = alarm_link->Alarm_node;
// 遍历链表并记录给数组
	while(alarm_index_node->next != NULL){
		uint32_t alarm_count = alarm_index_node->next->cnt;
		alarm_element_date[i].hour = alarm_count / 3600 % 24;
		alarm_element_date[i].min = alarm_count / 60 % 60;
		alarm_element_date[i].sec = alarm_index_node->next->sec;
		i++;
		alarm_index_node = alarm_index_node->next;
	}
	return;
}

/**
	*函数：返回头链表时刻内容
	*参数：无
	*返回值：目前第一个要触发的时刻
	*说明：直接返回第一个任务的时刻，如果没有闹钟任务就返回满值
*/
uint32_t alarm_link_headCount(void){
	if(alarm_link->quantity == 0)
		return 0xFFFFFFFF;
	return alarm_link->Alarm_node->next->cnt;
}

/**
	*函数：返回未尾链表时刻内容
	*参数：无
	*返回值：目前列表最后一个要触发的时刻
	*说明：直接返回最后一个任务的时刻，如果没有闹钟任务就返回满值
*/
uint32_t alarm_link_tailCount(void){
	if(alarm_link->quantity == 0)
		return 0xFFFFFFFF;
// 定义移动次数变量和索引节点
	uint8_t i = alarm_link->quantity;
	ALARM_NODE_TYPE alarm_link_index = alarm_link->Alarm_node;
// 移动操作
	while(--i){
		alarm_link_index = alarm_link_index->next;
	}
// 返回末尾节点的值
	return alarm_link_index->next->cnt;
}

/**
	*函数：将第一个执行的任务调到最后一个
	*参数：无
	*返回值：无
	*说明：将第一个任务调到最后一个任务，常用于执行后调用
*/
void alarm_link_cycle(void){
// 不允许空链表来运行
	if(alarm_link->quantity == 0)
		return;
// 如果链表只有一个元素
	if(alarm_link->quantity == 1){
		alarm_link->Alarm_node->next->cnt += 60*60*24;
		return;
	}
// 定义索引节点和需要移动的节点
	ALARM_NODE_TYPE alarm_index_node = alarm_link->Alarm_node;
	ALARM_NODE_TYPE alarm_link_move = alarm_index_node->next;
// 移动索引链表
	alarm_index_node = alarm_index_node->next;
	while(alarm_index_node->next != NULL){
		alarm_index_node = alarm_index_node->next;
	}		// 移动到链表末端
// 链表末端指向需要移动的节点
	alarm_index_node->next = alarm_link_move;
// 头节点指向第二个
	alarm_link->Alarm_node->next = alarm_link_move->next;
// 顺延到末端的节点下一个指向空
	alarm_link_move->next = NULL;
// 更新需要移动的节点
	alarm_link_move->cnt += 60*60*24;
	return;
}


/**
	*函数：将最后一个执行的任务调到第一个
	*参数：无
	*返回值：无
	*说明：将最后一个任务调到第一个任务，常用于执行前调用
*/
void alarm_link_rotationCycle(void){
// 不允许空链表来运行
	if(alarm_link->quantity == 0)
		return;
// 如果链表只有一个元素
	if(alarm_link->quantity == 1){
		alarm_link->Alarm_node->next->cnt -= 60*60*24;
		return;
	}
// 定义索引节点和需要移动的节点以及所需标记节点
	ALARM_NODE_TYPE alarm_link_head = alarm_link->Alarm_node;
	ALARM_NODE_TYPE alarm_link_move_last = alarm_link_head;
	ALARM_NODE_TYPE alarm_link_move = alarm_link_head->next;
// 移动需要移动的节点以及所需标记节点的位置
	while(alarm_link_move->next != NULL){
		alarm_link_move_last = alarm_link_move;
		alarm_link_move = alarm_link_move->next;
	}
// 进行链表末节点调换第一
	alarm_link_move->next = alarm_link_head->next;
	alarm_link_head->next = alarm_link_move;
	alarm_link_move_last->next = NULL;
// 更新移动过的节点
	alarm_link_move->cnt -= 60*60*24;
	return;
}

/**
	*函数：获取第一个任务的电机秒数
	*参数：无
	*返回值：目前第一个将执行的闹钟需要运行电机的秒数
	*说明：如果没有任务就直接返回0，有任务就返回第一个闹钟的电机秒数
*/
uint8_t alarm_link_headSecond(void){
	if(alarm_link->quantity == 0)
		return 0;
	return alarm_link->Alarm_node->next->sec;
}
