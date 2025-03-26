#include <stm32f4xx.h>
#include "fingerprint.h"
#include "usart.h"
#include "led.h"

extern uint8_t finger_rcv_msg[1024]={0};	//usart6接收数据缓冲区
extern uint8_t finger_rcv_count=0;			//usart6接收指令长度
extern uint8_t finger_rcv_flag=0;			//一条指令存贮完成标志位：1表示完成
extern uint8_t finger_send_cmd[1024]={0};	//从usart6发出给指纹模块的指令

/*--------------------------- 指纹模块指令集 ---------------------------*/
// 验证指令：bit0（0xF5）        校验位：bit6（bit1~bit5异或）
extern uint8_t Verify[8]={0xf5,0x0c,0,0,0,0,0x0c,0xf5};			//比对1：N
extern uint8_t Add0_1[8]={0xf5,0x01,0,0x01,0x01,0,0x01,0xf5};
extern uint8_t Add0_2[8]={0xf5,0x02,0,0x01,0x01,0,0x02,0xf5};
extern uint8_t Add0_3[8]={0xf5,0x03,0,0x01,0x01,0,0x03,0xf5};
extern uint8_t Add1_1[8]={0xf5,0x01,0,0x02,0x02,0,0x01,0xf5};
extern uint8_t Add1_2[8]={0xf5,0x02,0,0x02,0x02,0,0x02,0xf5};
extern uint8_t Add1_3[8]={0xf5,0x03,0,0x02,0x02,0,0x03,0xf5};
extern uint8_t Add2_1[8]={0xf5,0x01,0,0x03,0x02,0,0x00,0xf5};
extern uint8_t Add2_2[8]={0xf5,0x02,0,0x03,0x02,0,0x03,0xf5};
extern uint8_t Add2_3[8]={0xf5,0x03,0,0x03,0x02,0,0x02,0xf5};
extern uint8_t Add3_1[8]={0xf5,0x01,0,0x04,0x02,0,0x07,0xf5};
extern uint8_t Add3_2[8]={0xf5,0x02,0,0x04,0x02,0,0x04,0xf5};
extern uint8_t Add3_3[8]={0xf5,0x03,0,0x04,0x02,0,0x05,0xf5};
extern uint8_t Add4_1[8]={0xf5,0x01,0,0x05,0x02,0,0x06,0xf5};
extern uint8_t Add4_2[8]={0xf5,0x02,0,0x05,0x02,0,0x05,0xf5};
extern uint8_t Add4_3[8]={0xf5,0x03,0,0x05,0x02,0,0x04,0xf5};
extern uint8_t Delete_all[8]={0xf5,0x05,0,0,0,0,0x05,0xf5};		// 删除所有用户
extern uint8_t Get_count[8]={0xf5,0x09,0,0,0,0,0x09,0xf5};		// 获取用户数量


void usart6_finger_init()
{
	USART_InitTypeDef USART_InitStruct={0};
	GPIO_InitTypeDef GPIO_InitStruct={0};
	NVIC_InitTypeDef NVIC_InitStruct={0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	/* 1.初始化串口*/
	USART_InitStruct.USART_BaudRate		=19200;
	USART_InitStruct.USART_Mode			=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity		=USART_Parity_No;
	USART_InitStruct.USART_WordLength	=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits		=USART_StopBits_1;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART6,&USART_InitStruct);
	
	/* 2.复用GPIO */
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);

	/* 3.初始化中断*/
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
	USART_ClearFlag(USART6,USART_FLAG_TC);

	NVIC_InitStruct.NVIC_IRQChannel				=USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd			=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=2;
	NVIC_Init(&NVIC_InitStruct);
	
	/* 4.串口使能*/
	USART_Cmd(USART6,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void Usart6_SendByte(uint8_t data)
{
    USART_SendData(USART6,data);
    while(USART_GetFlagStatus(USART6,USART_FLAG_TXE) != SET){__NOP();}
}

void Usart6_SendBytes(uint8_t *str,uint16_t n)
{
	for(uint16_t i=0;i<n;i++)
	{
		Usart6_SendByte(str[i]);
	}
}

void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6,USART_IT_RXNE)==SET)
	{
		uint8_t byte = USART_ReceiveData(USART6);

        if (finger_rcv_count == 0 && byte != 0xF5) return;//包头检测

		finger_rcv_msg[finger_rcv_count++]=byte;
		
        if(finger_rcv_count > 7)
        {
            if(finger_rcv_msg[7] == 0xF5) // 包尾校验
            {
                finger_rcv_flag = 1;
            }
            finger_rcv_count = 0; // 无论是否合法都重置
        }
		
		if(finger_rcv_count>1023)
		{
			finger_rcv_count=0;
		}
		
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
	}
}

//获取用户数量
uint8_t finger_usernum(void)
{
	Usart6_SendBytes(Get_count, 8);
	finger_rcv_flag=0;
	return finger_rcv_msg[3];
}

//添加用户（user0,user1,user2）
void finger_useradd(user_num user)
{
	typedef enum {IDLE, STEP1, STEP2, STEP3} enroll_state;
	enroll_state fflag=IDLE;

	while(1)
	{
		if(fflag == IDLE && user == USER0)
		{
			Usart6_SendBytes(Add0_1, 8);
			led_ctrl(led_0, led_on);
			fflag = STEP1;
			finger_rcv_flag = 0; // 明确清除标志
		}
		else if(fflag == IDLE && user == USER1)
		{
			Usart6_SendBytes(Add1_1, 8);
			led_ctrl(led_0, led_on);
			fflag = STEP1;
			finger_rcv_flag = 0; // 明确清除标志
		}
		else if(fflag == IDLE && user == USER2)
		{
			Usart6_SendBytes(Add2_1, 8);
			led_ctrl(led_0, led_on);
			fflag = STEP1;
			finger_rcv_flag = 0; // 明确清除标志
		}

		// 状态机处理
		if(finger_rcv_flag == 1)
		{
			if(finger_rcv_msg[4]==0x08)return;
			switch(fflag)
			{
				case STEP1:
					if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x01 && user == USER0)
					{
						Usart6_SendBytes(Add0_2, 8);
						//led_ctrl(led_1, led_on);
						fflag = STEP2;
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x01 && user == USER1)
					{
						Usart6_SendBytes(Add1_2, 8);
						led_ctrl(led_1, led_on);
						fflag = STEP2;
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x01 && user == USER2)
					{
						Usart6_SendBytes(Add2_2, 8);
						//led_ctrl(led_1, led_on);
						fflag = STEP2;
					}
					finger_rcv_flag = 0;
					break;
				
				case STEP2:
					if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x02 && user == USER0)
					{
						Usart6_SendBytes(Add0_3, 8);
						//led_ctrl(led_2, led_on);
						fflag = STEP3;
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x02 && user == USER1)
					{
						Usart6_SendBytes(Add1_3, 8);
						led_ctrl(led_2, led_on);
						fflag = STEP3;
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x02 && user == USER2)
					{
						Usart6_SendBytes(Add2_3, 8);
						//led_ctrl(led_2, led_on);
						fflag = STEP3;
					}
					finger_rcv_flag = 0;
					break;
				
				case STEP3:
					if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x03 && user == USER0)
					{
						// 录入成功
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user0 add success\n\r");
						return; // 返回
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x03 && user == USER1)
					{
						// 录入成功
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user1 add success\n\r");
						return; // 返回
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x03 && user == USER2)
					{
						// 录入成功
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user2 add success\n\r");
						return; // 返回
					}
					finger_rcv_flag = 0;
					break;
			}
		}
	}
}

void finger_compere(void)
{
	Usart6_SendBytes(Verify,8);
	finger_rcv_flag=0;
	while(1)
	{
		if(finger_rcv_flag==1&&(finger_rcv_msg[4]==0x01||finger_rcv_msg[4]==0x02||finger_rcv_msg[4]==0x03))//判断比对指纹是否有对应用户的权限
		{
			for(int i=0;i<3;i++)
			{
				led_ctrl(led_0,led_on);
				led_ctrl(led_1,led_on);
				led_ctrl(led_2,led_on);
				delay_ms(200);
				led_ctrl(led_0,led_off);
				led_ctrl(led_1,led_off);
				led_ctrl(led_2,led_off);
				delay_ms(200);
			}
			return ;
		}
		else if(finger_rcv_flag==1&&finger_rcv_msg[4]==0x05)//比对失败（没有该用户）
		{
			led_ctrl(led_0,led_on);
			led_ctrl(led_1,led_on);
			led_ctrl(led_2,led_on);
			delay_ms(1000);
			led_ctrl(led_0,led_off);
			led_ctrl(led_1,led_off);
			led_ctrl(led_2,led_off);
			return ;
		}
		else if(finger_rcv_flag==1&&finger_rcv_msg[4]==0x08)//比对超时
		{
			return ;
		}
	}
}

