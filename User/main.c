/* encoding: utf-8 */
/*
 * 电路逻辑部分：
 * 左中右按钮分别接 PA10 PA11 PA12
 * 9-TB6612电机驱动 PWMA接PA8 AIN2接PA5 AIN1接PA4 STBY接PA3
 *                  VM接5V VCC接3.3V GND接GND AIO两端接电机
 * 3.3V给VBT接上
*/
#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Machine.h"
#include "Timer.h"
#include "Display.h"
#include "Alarm.h"
#include "MyKey.h"

#include "stdio.h"
#include "string.h"

#define X_font 8
#define Y_font 16

uint32_t alarm_cnt;
uint8_t timer_cnt = 0, timer_max;
// 主函数
int main(void){
// 初始化模块
	OLED_Init();
	MyRTC_Init();
	Machine_Init();
	Timer_Init();
	MyKey_Init();
	
// 初始化外设配置
	Machine_config_init();
	
// 初始化闹钟和显示的存储
	alarm_link_init();
	display_stack_init();

// 主循环所需变量
	DISPLAY_TYPE current_status;
	uint8_t current_select;
	uint8_t key_num;
	char s[] = "X.XX:XX XXs";
	char time_cur[16];
	while(1){
		OLED_Clear();
		// 扫描按键
		key_num = MyKey_Scan();
		display_keyEvent(key_num);
		// 读取常用数据
		current_status = display_GetStatus();
		current_select = display_GetSelect();
		// 读取当前实时时间
		MyRTC_ReadTime();
		// 判断页面状态
		switch(current_status){
			// 主页的情况
			case MAIN_MENUE:
				sprintf(time_cur, "时间：%02d:%02d:%02d", MyRTC_Time.hour, MyRTC_Time.minute, MyRTC_Time.second);
				OLED_ShowString(0, 0, time_cur, OLED_8X16);											// 显示为"时间：00:00:00"
				OLED_ShowString(1*X_font, 1*Y_font, "1.设定时间", OLED_8X16);
				OLED_ShowString(1*X_font, 2*Y_font, "2.查看时间", OLED_8X16);
				OLED_ShowString(1*X_font, 3*Y_font, "3.删除时间", OLED_8X16);
				OLED_ShowChar(0, (current_select + 1) *Y_font, '*', OLED_8X16);
				break;
			
	
			// 设定时间1的情况
			case SET_TIME_1:
				OLED_ShowString(0, 0, "设定时间：", OLED_8X16);
				OLED_ShowString(4*X_font, 2*Y_font, "0X:XX", OLED_8X16);
				for(uint8_t i=0; i <= set_time_state; i++){
					if(i == 0)
						OLED_ShowNum(4*X_font, 2*Y_font, temp_time.hour / 10, 1, OLED_8X16);
					if(i == 1)
						OLED_ShowNum(5*X_font, 2*Y_font, temp_time.hour % 10, 1, OLED_8X16);
					if(i == 2)
						OLED_ShowNum(7*X_font, 2*Y_font, temp_time.min / 10, 1, OLED_8X16);
				}
				if(set_time_state <= 1)
					OLED_ShowNum((4+set_time_state)*X_font, 2*Y_font, current_select, 1, OLED_8X16);
				else
					OLED_ShowNum((5+set_time_state)*X_font, 2*Y_font, current_select, 1, OLED_8X16);
				break;
				
			// 设定时间2的情况
			case SET_TIME_2:
				OLED_ShowString(0, 0, "设定时间：", OLED_8X16);
				OLED_ShowString(2*X_font, 1*Y_font, "是否设定：", OLED_8X16);
				OLED_ShowString(2*X_font, 2*Y_font, "  是  否", OLED_8X16);
				OLED_ShowChar((2+current_select*4)*X_font, 2*Y_font, '*', OLED_8X16);
				break;
			
			// 查看时间1的情况
			case CHECK_TIME_1:
				alarm_link_read();
				for(uint8_t i=0; i<3; i++){
					OLED_ShowString(2*X_font, i*Y_font, "X.XX:XX XXs", OLED_8X16);
					OLED_ShowChar(2*X_font, i*Y_font, '1'+i, OLED_8X16);
				}
				if(alarm_link->quantity >= 1){
					sprintf(s, "1.%02d:%02d %02ds", alarm_element_date[0].hour, alarm_element_date[0].min, alarm_element_date[0].sec); 
					OLED_ShowString(2*X_font, 0*Y_font, s, OLED_8X16);
				}
				if(alarm_link->quantity >= 2){
					sprintf(s, "2.%02d:%02d %02ds", alarm_element_date[1].hour, alarm_element_date[1].min, alarm_element_date[1].sec); 
					OLED_ShowString(2*X_font, 1*Y_font, s, OLED_8X16);
				}
				if(alarm_link->quantity >= 3){
					sprintf(s, "3.%02d:%02d %02ds", alarm_element_date[2].hour, alarm_element_date[2].min, alarm_element_date[2].sec); 
					OLED_ShowString(2*X_font, 2*Y_font, s, OLED_8X16);
				}
				OLED_ShowString(0*X_font, 3*Y_font, "  是  否添加闹钟", OLED_8X16);
				OLED_ShowChar(current_select*4*X_font, 3*Y_font, '*', OLED_8X16);
				break;
				
			// 查看时间2的情况
			case CHECK_TIME_2:
				OLED_ShowString(0, 0, "添加闹钟：", OLED_8X16);
				OLED_ShowString(2*X_font, 2*Y_font, "0X:XX Xs", OLED_8X16);
				for(uint8_t i=0; i <= set_time_state; i++){
					if(i == 0){
						OLED_ShowNum(2*X_font, 2*Y_font, temp_time.hour / 10, 1, OLED_8X16);
					}
					if(i == 1){
						OLED_ShowNum(3*X_font, 2*Y_font, temp_time.hour % 10, 1, OLED_8X16);
					}
					if(i == 2){
						OLED_ShowNum(5*X_font, 2*Y_font, temp_time.min / 10, 1, OLED_8X16);
					}
					if(i == 3){
						OLED_ShowNum(6*X_font, 2*Y_font, temp_time.min % 10, 1, OLED_8X16);
					}
				}
				if(set_time_state <= 1)
					OLED_ShowNum((2+set_time_state)*X_font, 2*Y_font, current_select, 1, OLED_8X16);
				else if(set_time_state <= 3)
					OLED_ShowNum((3+set_time_state)*X_font, 2*Y_font, current_select, 1, OLED_8X16);
				else
					OLED_ShowNum(8*X_font, 2*Y_font, current_select, 1, OLED_8X16);
				break;
			
			// 删除闹钟1的情况
			case DELET_TIME_1:
				alarm_link_read();
				OLED_ShowString(0*X_font, 0*Y_font, "删除闹钟：", OLED_8X16);
				for(uint8_t i=0; i<3; i++){
					OLED_ShowString(0*X_font, (1+i)*Y_font, "  X.XX:XX XXs", OLED_8X16);
					OLED_ShowChar(2*X_font, (1+i)*Y_font, '1'+i, OLED_8X16);
				}
				if(alarm_link->quantity >= 1){
					sprintf(s, "1.%02d:%02d %02ds", alarm_element_date[0].hour, alarm_element_date[0].min, alarm_element_date[0].sec); 
					OLED_ShowString(2*X_font, 1*Y_font, s, OLED_8X16);
				}
				if(alarm_link->quantity >= 2){
					sprintf(s, "2.%02d:%02d %02ds", alarm_element_date[1].hour, alarm_element_date[1].min, alarm_element_date[1].sec); 
					OLED_ShowString(2*X_font, 2*Y_font, s, OLED_8X16);
				}
				if(alarm_link->quantity >= 3){
					sprintf(s, "3.%02d:%02d %02ds", alarm_element_date[2].hour, alarm_element_date[2].min, alarm_element_date[2].sec); 
					OLED_ShowString(2*X_font, 3*Y_font, s, OLED_8X16);
				}
				if(current_select > 0)
					OLED_ShowChar(0*X_font, (current_select)*Y_font, '*', OLED_8X16);
				break;
				
			// 删除闹钟2的情况
			case DELET_TIME_2:
				OLED_ShowString(0*X_font, 0*Y_font, "删除闹钟：", OLED_8X16);
				OLED_ShowString(2*X_font, 1*Y_font, "是否删除闹钟", OLED_8X16);
				OLED_ShowString(2*X_font, 2*Y_font, "  是  否", OLED_8X16);
				OLED_ShowChar((2+4*current_select)*X_font, 2*Y_font, '*', OLED_8X16);
				break;
		}
		OLED_Update();
	}
}

void RTCAlarm_IRQHandler(void){
	if(RTC_GetITStatus(RTC_IT_ALR) == SET){
	// 清空中断标志
		EXTI_ClearITPendingBit(EXTI_Line17);
		RTC_ClearITPendingBit(RTC_IT_ALR);
	// 记录时间
		timer_max = alarm_link_headSecond() - 1;
	// 开启时钟中断并开启电机
		TIM_Cmd(TIM2, ENABLE);
		Machine_ON();
	// 已处理的任务顺延后面
		alarm_link_cycle();
	// 设置下一个闹钟
		RTC_SetAlarm(alarm_link_headCount());
		RTC_WaitForLastTask();
	}
}
//	if(RTC_GetITStatus(RTC_IT_ALR) == SET){
//		EXTI_ClearITPendingBit(EXTI_Line17);
//		RTC_ClearFlag(RTC_FLAG_ALR);
//		RTC_ClearITPendingBit(RTC_IT_ALR);
//		
//		timer_max = alarm_link_headSecond();
//		TIM_Cmd(TIM2, ENABLE);
//		Machine_ON();
//		alarm_link_cycle();
//		RTC_EnterConfigMode();
//		RTC_SetAlarm(alarm_link_headCount());
//		RTC_WaitForLastTask();
//		RTC_ExitConfigMode();
//	}
//	RTC_ClearITPendingBit(RTC_IT_OW);
//	RTC_WaitForLastTask();
//}

void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)){
		if(++timer_cnt >= timer_max){
			timer_cnt = 0;
			TIM_Cmd(TIM2, DISABLE);
			Machine_OFF();
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
