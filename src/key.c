#include <stm32f4xx.h>
#include "led.h"
#include "key.h"
#include "delay.h"

void somedelay(void)
{
	int i = 0;

	for(i=0;i<0xfffff;i++);
}

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	//使能GPIO端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	//初始化GPIO
	GPIO_InitStruct.GPIO_Pin	=	GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;//按键1�?�?�?
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd	=	GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
}

press_key key_scan(void)
{
	uint8_t KEY0=GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9);
	uint8_t KEY1=GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8);
	uint8_t KEY2=GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7);
	uint8_t KEY3=GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6);
	
	if(KEY0==0||KEY1==0||KEY2==0||KEY3==0)
	{
		if(KEY0==0)			{delay_ms(20);return key_0;}
		else if(KEY1==0)	{delay_ms(20);return key_1;}
		else if(KEY2==0)	{delay_ms(20);return key_2;}
		else if(KEY3==0)	{delay_ms(20);return key_3;}
	}
}

void KeyExtiInit(void)
{
    /* 1.配置按键对应的GPIO引脚为通用输入模式 */
	GPIO_InitTypeDef GPIO_InitStruct={0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	//初始化GPIO
	GPIO_InitStruct.GPIO_Pin	=	GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_IN;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
    
    /* 2.选择按键对应的GPIO引脚作为外部中断输入信号�?*/
	//使能SYSCFG的时�?
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	//选择对应引脚作为外部中断输入信号
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource6);
	
    /* 3.配置外部中断控制�?*/
    EXTI_InitTypeDef EXTI_InitStruct = {0};

	EXTI_InitStruct.EXTI_Line		=	EXTI_Line6;
	EXTI_InitStruct.EXTI_Mode		=	EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger	=	EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd	=	ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
    /* 4.配置NVIC */
	NVIC_InitTypeDef  NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI9_5_IRQHandler(void)
{
    //由于有些中断有多种触发方式，通常会在中断处理函数中再做一次具体的判断
    if(EXTI_GetITStatus(EXTI_Line6) == SET)	//表示EXTIn中断被触�?
    {
		
       	EXTI_ClearITPendingBit(EXTI_Line6);//清除EXTIn中断标志 
    }
}

