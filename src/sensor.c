#include <stm32f4xx.h>
#include "sensor.h"
#include "usart.h"
#include "led.h"

extern uint8_t rsv_msg[1024]={0};
extern uint8_t rsv_index=0;
extern uint16_t flag=0;

void usart3_sensor_init()
{
	USART_InitTypeDef USART_InitStruct={0};
	GPIO_InitTypeDef GPIO_InitStruct={0};
	NVIC_InitTypeDef NVIC_InitStruct={0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	/* 1.初始化USART3:数据帧和速率必须与PC上的一致 */
	USART_InitStruct.USART_BaudRate		=9600;
	USART_InitStruct.USART_Mode			=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity		=USART_Parity_No;
	USART_InitStruct.USART_WordLength	=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits		=USART_StopBits_1;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStruct);
	
	/* 2.配置GPIO引脚复用功能为USART1 */
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);

	/* 3.配置当 RXNE置1时触发中断 */
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel				=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd			=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=2;
	NVIC_Init(&NVIC_InitStruct);
	
	/* 4.开启串口 */
	USART_Cmd(USART3,ENABLE);
}

void Usart3_SendByte(uint8_t data)
{
    USART_SendData(USART3,data);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) != SET){__NOP();}
}

//串口1发送1串数据
void Usart3_SendBytes(uint8_t *str,uint16_t n)
{
	for(uint16_t i=0;i<n;i++)
	{
		Usart3_SendByte(str[i]);
	}
}

//uint8_t rsv_msg[1024]={0};
//uint8_t rsv_index=0;
//uint16_t flag=0;

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET)
	{
		uint8_t byte = USART_ReceiveData(USART3);

        // 1. 检查帧头
        if (rsv_index == 0 && byte != 0x5A) return;
        if (rsv_index == 1 && byte != 0x5A) 
		{
            rsv_index = 0;
            return;
        }

		rsv_msg[rsv_index++]=byte;
		
		if(rsv_msg[rsv_index-1]=='\n' && rsv_msg[rsv_index-2]=='\r')
        {
            flag = rsv_index;
			rsv_index = 0;
        }
		if(rsv_index>1023)
		{
			rsv_index=0;
		}
		
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
}

void GY39_RequestLight(void) 
{
    uint8_t cmd[] = {0xA5, 0x81, 0x26};  // 根据实际协议修改指令
    Usart3_SendBytes(cmd, sizeof(cmd));
}


