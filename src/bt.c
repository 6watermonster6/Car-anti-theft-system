#include <stm32f4xx.h>
#include "bt.h"
#include "led.h"
#include "can.h"

extern uint8_t bt_rcv_msg[1024]={0};	//usart3接收数据缓冲区
extern uint8_t bt_rcv_count=0;			//usart3接收指令长度
extern uint8_t bt_rcv_flag=0;			//一条指令存贮完成标志位：1表示完成

void bt_init(void)
{
	// 启用USART3和GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	USART_InitTypeDef USART_InitStruct={0};
	GPIO_InitTypeDef GPIO_InitStruct={0};
	NVIC_InitTypeDef NVIC_InitStruct={0};

	// 配置TX(PB10)和RX(PB11)引脚
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;// 复用推挽输出
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	// 配置USART3参数
	USART_InitStruct.USART_BaudRate = 9600; 		 // 与HC-05波特率一致
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);

	// 启用接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel				=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd			=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=2;
	NVIC_Init(&NVIC_InitStruct);
}

void Usart3_SendByte(uint8_t data)
{
    USART_SendData(USART3,data);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) != SET){__NOP();}
}

void Usart3_SendBytes(uint8_t *str,int n)
{
	for(int i=0;i<n;i++)
	{
		Usart3_SendByte(str[i]);
	}
}

// 中断服务函数
void USART3_IRQHandler(void) 
{
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
  {
	uint8_t byte = USART_ReceiveData(USART3);

	if (bt_rcv_count == 0 && byte != 0xA5) return;//包头检测
	
	bt_rcv_msg[bt_rcv_count++]=byte;
	
	if(bt_rcv_count > 7)
	{
		if(bt_rcv_msg[7] == 0x5A) // 包尾校验
		{
			if(bt_rcv_msg[1]==0xf5)
			{
				CanTxMsg sendto_can={0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0,0,0,0,0,0,0xf5}};
				for(int i=1;i<=3;i++)
				{
					sendto_can.Data[i]=bt_rcv_msg[i+1];
				}
				CAN_SendData(CAN1,&sendto_can);
				bt_rcv_flag = 1;
			}
		}
		bt_rcv_count = 0; // 无论是否合法都重置
	}
	
	if(bt_rcv_count>1023)
	{
		bt_rcv_count=0;
	}

    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
  }
}



