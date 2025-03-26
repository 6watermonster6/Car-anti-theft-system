#include <stm32f4xx.h>
#include "led.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	//使能GPIO端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);

	//初始化GPIO
	GPIO_InitStruct.GPIO_Pin	=	GPIO_Pin_3|GPIO_Pin_4;	//PE3,PE4
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	=	GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=	GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	=	GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin	=	GPIO_Pin_9;				//PG9
	GPIO_Init(GPIOG,&GPIO_InitStruct);

	//初始状态
	GPIO_SetBits(GPIOE,GPIO_Pin_3|GPIO_Pin_4);
	GPIO_SetBits(GPIOG,GPIO_Pin_9);
}

void led_ctrl(Led_num led,Led_state state)
{

	switch (led)
		{
			case led_0 :
				if(state==led_on){
					GPIO_ResetBits(GPIOE,GPIO_Pin_3);
					}
				else{
					GPIO_SetBits(GPIOE,GPIO_Pin_3);
					}
				break;
			case led_1 :
				if(state==led_on){
					GPIO_ResetBits(GPIOE,GPIO_Pin_4);
					}
				else{
					GPIO_SetBits(GPIOE,GPIO_Pin_4);
					}
				break;
			case led_2 :
				if(state==led_on){
					GPIO_ResetBits(GPIOG,GPIO_Pin_9);
					}
				else{
					GPIO_SetBits(GPIOG,GPIO_Pin_9);
					}
				break;
		}
}


