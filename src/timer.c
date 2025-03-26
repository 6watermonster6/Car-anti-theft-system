#include <stm32f4xx.h>
#include "timer.h"
#include "usart.h"

void timer2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};
	TIM_TimeBaseInitStruct.TIM_Prescaler		=	8399;
	TIM_TimeBaseInitStruct.TIM_CounterMode		=	TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period			=	9999;
	TIM_TimeBaseInitStruct.TIM_ClockDivision	=	0;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter=	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

	NVIC_InitTypeDef  NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)	//表示EXTIn中断被触发
    {
       	//该中断触发时要执行的代码
		//Usart1_SendBytes("MVP",3);
		//GPIO_ToggleBits(GPIOE,GPIO_Pin_4);
		//GPIO_ToggleBits(GPIOE,GPIO_Pin_3);//翻转bit位
       	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除EXTIn中断标志
    }
}

void tim_pwm_init()
{

	//初始化时基单元
	
}


