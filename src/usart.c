#include <stm32f4xx.h>
#include "usart.h"

void usart1_init()
{
	/* 1.初始化USART1:数据帧和速率必须与PC上的一�?*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate		=9600;
	USART_InitStruct.USART_Mode			=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity		=USART_Parity_No;
	USART_InitStruct.USART_WordLength	=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits		=USART_StopBits_1;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStruct);
	
	/* 2.配置GPIO引脚复用功能为USART1 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct={0};
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	/* 3.配置�?RXNE�?时触发中�?*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel				=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd			=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=2;
	NVIC_Init(&NVIC_InitStruct);
	
	/* 4.开启串�?*/
	USART_Cmd(USART1,ENABLE);
}
	
	
//串口1发�?个字�?
void Usart1_SendByte(uint8_t data)
{
    USART_SendData(USART1,data);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) != SET){__NOP();}
}

//串口1发�?串数�?
void Usart1_SendBytes(uint8_t *str,int n)
{
	for(int i=0;i<n;i++)
	{
		Usart1_SendByte(str[i]);
	}
}

//串口1的中断处理函数：服务于串口接收数据，每当串口接收�?个字节，就会触发一次中断，CPU就可以接�?个字�?
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
		{
		uint16_t data = USART_ReceiveData(USART1);
		
		Usart1_SendByte(data);
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		}
}

#include <stdio.h>

#pragma import(__use_no_semihosting)  // ���ð�����

// ����ϵͳ�ӿ�
struct __FILE { int handle; };
FILE __stdout;

// ʵ��fputc
int fputc(int ch, FILE *f) {
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}

// ��ʵ��_sys_exit
void _sys_exit(int x) {
    x = x;  // �������������
}



