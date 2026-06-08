#include "stm32f10x.h"                  // Device header

void PWM_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
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
	OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	OC_InitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &OC_InitStruct);
	
	TIM_Cmd(TIM1, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare){
	TIM_SetCompare1(TIM1, Compare);
}

void PWM_SetPrescaler(uint16_t Prescaler){
	TIM_PrescalerConfig(TIM1, Prescaler, TIM_PSCReloadMode_Immediate);
}
