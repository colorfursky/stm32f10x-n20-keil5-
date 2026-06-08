/*
	所需材料：电机  TB6612FNG继电器模块  按钮*4 OLED模块
	
	接线：TB6612FNG: PWMA 接 PA8 , AIN2 接 PA5 , AIN1 接 PA4 , STBY 接 PA3
									VM 接 V5 , VCC 接V3.3 , GND 接 GND , AO1和AO2 接 电机
				按钮分别接 A8 A10 B10 B13
				OLED模块 SCL 接 PB8 , SDA 接 PB9
*/

#include "stm32f10x.h"                  // Device header
#include "Machine.h"

void Machine_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	
	TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TimeBase_InitStruct;
	TimeBase_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBase_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBase_InitStruct.TIM_Period = 100 - 1;   // ARR
	TimeBase_InitStruct.TIM_Prescaler = 720 - 1;  // PSC
	TimeBase_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TimeBase_InitStruct);
	
	TIM_OCInitTypeDef OC_InitStruct;
	TIM_OCStructInit(&OC_InitStruct);
	OC_InitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
	OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	OC_InitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &OC_InitStruct);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);
}

uint8_t Machine_CheckCmd(void){
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == RESET){
		return 0;
	}else{
		return 1;
	}
}

uint8_t Machine_CheckRotation(void){
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0){
		return 0;
	}else{
		return 1;
	}
}

void Machine_SetSpeed(uint8_t Percentage){
	TIM_SetCompare1(TIM1, Percentage);
}

void Machine_ON(void){
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void Machine_OFF(void){
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void Machine_TURN(void){
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET){
		Machine_OFF();
	}else{
		Machine_ON();
	}
}

void Machine_ForwardRotation(void){
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void Machine_ReverseRotation(void){
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void Machine_RotationTurn(void){
	if(Machine_CheckRotation() == 1){
		Machine_ReverseRotation();
	}else{
		Machine_ForwardRotation();
	}
}

void Machine_config_init(void){
	Machine_OFF();
	Machine_SetSpeed(INIT_SPEED);
	Machine_ForwardRotation();
}
