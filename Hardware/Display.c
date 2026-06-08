#include "stm32f10x.h"                  // Device header
#include "Display.h"
#include "Alarm.h"
#include "MyRTC.h"

#include <time.h>
#include <stdlib.h>

uint8_t set_time_state = 0;				// 临时存放设置时间/闹钟状态机
DISPLAY_INFO_STACK display_stack;		// 显示栈头节点
DATE_ELEMENT_TYPE temp_time = {0, 0};
uint8_t temp_second = 0;

/****工具函数******************/

/**
	*函数：设置界面目前选择
	*参数：选择设置值
	*返回值：无
	*说明：给显示的栈栈顶设置选择内容
*/
void display_stack_setSelect(uint8_t select){
	display_stack->stack->select = select;
}

/**
	*函数：返回当前界面选择的上限值
	*参数：界面类型
	*返回值：选择的上限
	*说明：目前是直接返回界面类型所对应的选择上限
*/
uint8_t Return_selectLimit(DISPLAY_TYPE type){
	switch(type){
		case MAIN_MENUE:
			return 2;
//			break;
		case SET_TIME_1:
			// 对于小时的十位数
			if(set_time_state == 0)
				return 2;
			// 对于小时的个位数
			if(set_time_state == 1){
				if(temp_time.hour >= 20){
					return 3;
				}else{
					return 9;
				}
			}
			// 对于分钟的十位数
			if(set_time_state == 2){
				return 5;
			}
			// 对于分钟的个位数
			if(set_time_state == 3){
				return 9;
			}
			break;
		case SET_TIME_2:
			return 1;
//			break;
		case CHECK_TIME_1:
			return 1;
//			break;
		case CHECK_TIME_2:
			// 对于小时的十位数
			if(set_time_state == 0)
				return 2;
			// 对于小时的个位数
			if(set_time_state == 1){
				if(temp_time.hour >= 20){
					return 3;
				}else{
					return 9;
				}
			}
			// 对于分钟的十位数
			if(set_time_state == 2){
				return 5;
			}
			// 对于分钟的个位数
			if(set_time_state == 3){
				return 9;
			}
			// 对于触发秒数
			if(set_time_state == 4){
				return 9;
			}
//			break;
		case DELET_TIME_1:
			return alarm_link->quantity;
//			break;
		case DELET_TIME_2:
			return 1;
//			break;
	}
	return 0;
}

/**
	*函数：将存放界面数据的栈初始化
	*参数：无
	*返回值：无
	*说明：初始化会给栈分配空间，并默认有个主要菜单在栈里
*/
void set_TimeAndAlarm(void){
// 定义时间设置前和设置后的count
	uint32_t old_time, new_time;
// 更新MyRTC_Time结构体
	MyRTC_ReadTime();
// 记录旧时间计数
	old_time = RTC_GetCounter();
// 改变时间
	MyRTC_Time.hour = temp_time.hour;
	MyRTC_Time.minute = temp_time.min;
	MyRTC_Time.second = 0;
	MyRTC_SetTime();
// 记录新时间计数
	new_time = RTC_GetCounter();
// 如果没有时钟就不调整
	if(alarm_link->quantity < 1)
		return;
// 调整闹钟
	if(new_time > old_time){			// 如果是前进改变
		while(new_time > alarm_link_headCount()){		// 检测是否跳过闹钟
			alarm_link_cycle();
		}
	}else{		// 如果是后退改变
		while(new_time < alarm_link_tailCount() - 60*60*24){		// 检测是否错过闹钟
			alarm_link_rotationCycle();
		}
	}
// 重置下一个闹钟
	RTC_SetAlarm(alarm_link_headCount());
	return;
}

/**
	*函数：返回确认按键发生跳转所跳转的位置，并处理事务
	*参数：当前显示栈栈顶
	*返回值：无
	*说明：会结合当前栈顶选择进行数据处理并跳转
*/
void enter_into(DISPLAY_INFO_STACK cur){
	DISPLAY_TYPE type = cur->stack->status;
	uint8_t select = cur->stack->select;
	uint32_t cnt;
	struct tm time_date;
	switch(type){
		case MAIN_MENUE:
			if(select == 0)
				 display_stack_push(SET_TIME_1);
			if(select == 1)
				display_stack_push(CHECK_TIME_1);
			if(select == 2)
				display_stack_push(DELET_TIME_1);
			break;
		case SET_TIME_1:
			display_stack_push(SET_TIME_2);
			break;
		case SET_TIME_2:
			if(select == 0){
				// 设置时间的逻辑
				set_TimeAndAlarm();
				temp_time.hour = 0; temp_time.min = 0;
				set_time_state = 0;
			}
			if(select == 1){
				// 没逻辑
				temp_time.hour = 0; temp_time.min = 0;
				set_time_state = 0;
			}
			// 返回主页
			display_stack_pop();
			display_stack_pop();
			break;
		case CHECK_TIME_1:
			if(select == 0)
				display_stack_push(CHECK_TIME_2);
			if(select == 1){
				display_stack_pop();
			}
			break;
		case CHECK_TIME_2:
		// 设定新闹钟的逻辑
			MyRTC_ReadTime();
			time_date.tm_year = MyRTC_Time.year - 1900;
			time_date.tm_mon = MyRTC_Time.month - 1;
			time_date.tm_mday = MyRTC_Time.day;
			time_date.tm_hour = temp_time.hour;
			time_date.tm_min = temp_time.min;
			time_date.tm_sec = 0;
			cnt = mktime(&time_date);
			if(cnt <= RTC_GetCounter())
				cnt += 60*60*24;
			alarm_link_insert(cnt, temp_second);
			temp_time.hour = 0; temp_time.min = 0;
			temp_second = 0;
		// 返回主页
//			while(display_stack->capacity != 1)
//					display_stack_pop();
			display_stack_pop();
			display_stack_pop();
			break;
		case DELET_TIME_1:
			if(select != 0){
				display_stack_push(DELET_TIME_2);
			}else{
				display_stack_pop();
			}
			break;
		case DELET_TIME_2:
			if(select == 0){
				// 删除闹钟的逻辑，利用预留机制
				alarm_link_delete(display_stack->stack->last->select);
				// 返回主页
				display_stack_pop();
				display_stack_pop();
			}
			if(select == 1){
				// 返回主页
				display_stack_pop();
				display_stack_pop();
			}
			break;
	}
}

/*****************工具函数*****/

/****功能函数******************/
/**
	*函数：将存放界面数据的栈初始化
	*参数：无
	*返回值：无
	*说明：初始化会给栈分配空间，并默认有个主要菜单在栈里
*/
void display_stack_init(void){
// 初始化显示数据结构
	display_stack = (DISPLAY_INFO_STACK) malloc(sizeof(struct DISPLAY_INFO_STACK));
	if(display_stack == NULL){
		while(1);
	}
	display_stack->capacity = DISPLAY_MAX_CAPACITY;
	display_stack->quantity = 1;
	display_stack->stack = (DISPLAY_INFO_TYPE) malloc(sizeof(struct DISPLAY_INFO));
	if(display_stack->stack == NULL){
		free(display_stack);
		while(1);
	}

// 初始化显示人机交互当前栈顶
	display_stack->stack->status = MAIN_MENUE;
	display_stack->stack->select = 0;
	display_stack->stack->last = NULL;
}

/**
	*函数：将进入界面状态压入栈中，并初始化新的栈数据
	*参数：压入栈的指定进入界面的状态，参数为 enter_status
	*返回值：返回0或1。即"Display.h"宏定义部分包含的 DISPLAY_PUSH_OK DISPLAY_PUSH_ERROR
	*说明：初始化会给栈分配空间，并默认有个主要菜单在栈里。常用于进入新的页面
*/
uint8_t display_stack_push(DISPLAY_TYPE enter_status){
// 检查显示栈的容量
	if(display_stack->quantity == display_stack->capacity){
		return DISPLAY_PUSH_ERROR;			// 返回错误
	}
	DISPLAY_INFO_TYPE new_display, old_display;					// 定义新节点和旧节点
	new_display = (DISPLAY_INFO_TYPE) malloc(sizeof(struct DISPLAY_INFO));		// 给新节点分配地址
	
// 检查是否分配成功
	if(new_display == NULL){
		while(1);
	}
	old_display = display_stack->stack;				// 赋值旧节点

// 新节点压入栈操作
	new_display->last = old_display;
	new_display->select = 0;
	new_display->status = enter_status;

// 更新栈状态
	display_stack->quantity++;
	display_stack->stack = new_display;
// 返回成功
	return DISPLAY_PUSH_OK;
}

/**
	*函数：将存放界面的栈定弹出，调整新的栈顶
	*参数：无
	*返回值：返回0或1。即"Display.h"宏定义部分包含的 DISPLAY_POP_OK DISPLAY_POP_ERROR
	*说明：弹出操作会先检测是否允许弹出，之后会弹出栈顶操作并更新栈状态。常用于退出界面
*/
uint8_t display_stack_pop(void){
// 不允许只剩一个元素时弹栈
	if(display_stack->quantity == 1)
		return DISPLAY_POP_ERROR;
	DISPLAY_INFO_TYPE p = display_stack->stack;					// 标记将弹出的节点
	display_stack->stack = display_stack->stack->last;	// 移动到上一个节点
	display_stack->quantity--;				// 更新栈状态
	free(p);							// 释放节点
	return DISPLAY_POP_OK;
}


/**
	*函数：获取当前界面状态
	*参数：无
	*返回值：返回枚举类型的界面状态 DISPLAY_TYPE
	*说明：直接返回用栈存储的栈顶界面状态内容
*/
DISPLAY_TYPE display_GetStatus(void){
	return display_stack->stack->status;
}

/**
	*函数：获取当前界面选择
	*参数：无
	*返回值：返回整型数
	*说明：直接返回用栈存储的栈顶选择内容数字
*/
uint8_t display_GetSelect(void){
	return display_stack->stack->select;
}

/**
	*函数：处理目前的界面状态下的按键事件
	*参数：按键码
	*返回值：无
	*说明：除了处理设定时间和查看闹钟的添加界面，其他都是直接跳转相关界面
*/
void display_keyEvent(uint8_t event){
// 按下左键逻辑
	if(event == KEY_LEFT_NUMBER){
		if(display_GetSelect() > 0)
			display_stack_setSelect(display_GetSelect() - 1);
	}
// 按下右键逻辑
	if(event == KEY_RIGHT_NUMBER){
		if(display_GetSelect() < Return_selectLimit(display_GetStatus())){
			display_stack_setSelect(display_GetSelect() + 1);
		}
	}
// 按下确认中键逻辑
	if(event == KEY_MIDDLE_NUMBER){
		if(display_stack->stack->status != SET_TIME_1 && display_stack->stack->status != CHECK_TIME_2)
			enter_into(display_stack);
		else{
			if(display_stack->stack->status == SET_TIME_1){
				// 设置时间处理逻辑
				switch(set_time_state){
					case 0:
						temp_time.hour += display_GetSelect()*10;
						display_stack_setSelect(0);
						set_time_state = 1;
						break;
					case 1:
						temp_time.hour += display_GetSelect();
						display_stack_setSelect(0);
						set_time_state = 2;
						break;
					case 2:
						temp_time.min += display_GetSelect()*10;
						display_stack_setSelect(0);
						set_time_state = 3;
						break;
					case 3:
						temp_time.min += display_GetSelect();
						display_stack_setSelect(0);
						enter_into(display_stack);
						break;
				}
			}
			if(display_stack->stack->status == CHECK_TIME_2){
				// 添加闹钟处理逻辑
				switch(set_time_state){
					case 0:
						temp_time.hour += display_GetSelect()*10;
						display_stack_setSelect(0);
						set_time_state = 1;
						break;
					case 1:
						temp_time.hour += display_GetSelect();
						display_stack_setSelect(0);
						set_time_state = 2;
						break;
					case 2:
						temp_time.min += display_GetSelect()*10;
						display_stack_setSelect(0);
						set_time_state = 3;
						break;
					case 3:
						temp_time.min += display_GetSelect();
						display_stack_setSelect(0);
						set_time_state = 4;
						break;
					case 4:
						temp_second = display_GetSelect();
						set_time_state = 0;
						display_stack_setSelect(0);
						enter_into(display_stack);
						break;
				}
			}
		}
	}
}
/******************功能函数****/
