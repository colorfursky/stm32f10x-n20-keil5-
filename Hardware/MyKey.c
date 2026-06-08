#include "stm32f10x.h"                  // Device header
#include "MyKey.h"

struct KEY_INFO key_left_data;			// KEY_LEFT 的内容
struct KEY_INFO key_middle_data;			// KEY_MIDDLE 的内容
struct KEY_INFO key_right_data;			// KEY_RIGHT 的内容
struct KEY_QUEUE key_queue;			// 按键任务循环队列

void MyKey_Keydata_Init(struct KEY_INFO * Key_data, uint8_t name, GPIO_TypeDef * Type, uint16_t Pin, uint8_t Count, uint8_t State){
	Key_data->Num_name = name;													// 设置按键任务名
	Key_data->Type = Type; Key_data->Pin = Pin;					// 设置按键对应引脚
	Key_data->Count = Count; Key_data->State = State;		// 设置按键状态机
}

void MyKey_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = KEY_LEFT | KEY_MIDDLE | KEY_RIGHT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	MyKey_Keydata_Init(&key_left_data, 1, GPIOA, KEY_LEFT, 0, 0);
	MyKey_Keydata_Init(&key_middle_data, 2, GPIOA, KEY_MIDDLE, 0, 0);
	MyKey_Keydata_Init(&key_right_data, 3, GPIOA, KEY_RIGHT, 0, 0);

	key_queue.p = 0; key_queue.q = 0;
	for(uint8_t i = 0; i<50; i++){
		key_queue.info[i] = 0;
	}
}

void Queue_push(uint8_t key_info){
	key_queue.info[key_queue.q++] = key_info;
	if(key_queue.q == 50){
		key_queue.q = 0;
	}
}

uint8_t Queue_Get(void){
	if(key_queue.p == key_queue.q)
		return 0;
	uint8_t result = key_queue.info[key_queue.p++];
	if(key_queue.p == 50)
		key_queue.p = 0;
	return result;
}

uint8_t MyKey_ReadPort(struct KEY_INFO *key_data){
	if(GPIO_ReadInputDataBit(key_data->Type, key_data->Pin) == Bit_SET){
		return 1;
	}else{
		return 0;
	}
}

void MyKey_StateChange(struct KEY_INFO *key_data){
	key_data->State += 1;
	if(key_data->State == 2){
		Queue_push(key_data->Num_name);
		key_data->State = 0;
	}
}

void MyKey_check(struct KEY_INFO *key_data){
	if(key_data->State == MyKey_ReadPort(key_data)){
		key_data->Count += 1;
		if(key_data->Count >= DELAY){
			key_data->Count = 0;
			MyKey_StateChange(key_data);
		}
	}else{
		key_data->Count = 0;
	}
}

uint8_t MyKey_Scan(void){
	MyKey_check(&key_left_data);
	MyKey_check(&key_middle_data);
	MyKey_check(&key_right_data);
	return Queue_Get();
}
