#include <stm32f4xx.h>
#include "fingerprint.h"
#include "usart.h"
#include "led.h"

extern uint8_t finger_rcv_msg[1024]={0};	//usart6�������ݻ�����
extern uint8_t finger_rcv_count=0;			//usart6����ָ���
extern uint8_t finger_rcv_flag=0;			//һ��ָ�������ɱ�־λ��1��ʾ���
extern uint8_t finger_send_cmd[1024]={0};	//��usart6������ָ��ģ���ָ��

/*--------------------------- ָ��ģ��ָ� ---------------------------*/
// ��ָ֤�bit0��0xF5��        У��λ��bit6��bit1~bit5���
extern uint8_t Verify[8]={0xf5,0x0c,0,0,0,0,0x0c,0xf5};			//�ȶ�1��N
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
extern uint8_t Delete_all[8]={0xf5,0x05,0,0,0,0,0x05,0xf5};		// ɾ�������û�
extern uint8_t Get_count[8]={0xf5,0x09,0,0,0,0,0x09,0xf5};		// ��ȡ�û�����


void usart6_finger_init()
{
	USART_InitTypeDef USART_InitStruct={0};
	GPIO_InitTypeDef GPIO_InitStruct={0};
	NVIC_InitTypeDef NVIC_InitStruct={0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	/* 1.��ʼ������*/
	USART_InitStruct.USART_BaudRate		=19200;
	USART_InitStruct.USART_Mode			=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity		=USART_Parity_No;
	USART_InitStruct.USART_WordLength	=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits		=USART_StopBits_1;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART6,&USART_InitStruct);
	
	/* 2.����GPIO */
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);

	/* 3.��ʼ���ж�*/
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
	USART_ClearFlag(USART6,USART_FLAG_TC);

	NVIC_InitStruct.NVIC_IRQChannel				=USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd			=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority	=2;
	NVIC_Init(&NVIC_InitStruct);
	
	/* 4.����ʹ��*/
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

        if (finger_rcv_count == 0 && byte != 0xF5) return;//��ͷ���

		finger_rcv_msg[finger_rcv_count++]=byte;
		
        if(finger_rcv_count > 7)
        {
            if(finger_rcv_msg[7] == 0xF5) // ��βУ��
            {
                finger_rcv_flag = 1;
            }
            finger_rcv_count = 0; // �����Ƿ�Ϸ�������
        }
		
		if(finger_rcv_count>1023)
		{
			finger_rcv_count=0;
		}
		
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
	}
}

//��ȡ�û�����
uint8_t finger_usernum(void)
{
	Usart6_SendBytes(Get_count, 8);
	finger_rcv_flag=0;
	return finger_rcv_msg[3];
}

//����û���user0,user1,user2��
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
			finger_rcv_flag = 0; // ��ȷ�����־
		}
		else if(fflag == IDLE && user == USER1)
		{
			Usart6_SendBytes(Add1_1, 8);
			led_ctrl(led_0, led_on);
			fflag = STEP1;
			finger_rcv_flag = 0; // ��ȷ�����־
		}
		else if(fflag == IDLE && user == USER2)
		{
			Usart6_SendBytes(Add2_1, 8);
			led_ctrl(led_0, led_on);
			fflag = STEP1;
			finger_rcv_flag = 0; // ��ȷ�����־
		}

		// ״̬������
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
						// ¼��ɹ�
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user0 add success\n\r");
						return; // ����
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x03 && user == USER1)
					{
						// ¼��ɹ�
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user1 add success\n\r");
						return; // ����
					}
					else if(finger_rcv_msg[0]==0xF5 && finger_rcv_msg[1]==0x03 && user == USER2)
					{
						// ¼��ɹ�
						led_ctrl(led_0, led_off);
						led_ctrl(led_1, led_off);
						led_ctrl(led_2, led_off);
						printf("user2 add success\n\r");
						return; // ����
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
		if(finger_rcv_flag==1&&(finger_rcv_msg[4]==0x01||finger_rcv_msg[4]==0x02||finger_rcv_msg[4]==0x03))//�жϱȶ�ָ���Ƿ��ж�Ӧ�û���Ȩ��
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
		else if(finger_rcv_flag==1&&finger_rcv_msg[4]==0x05)//�ȶ�ʧ�ܣ�û�и��û���
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
		else if(finger_rcv_flag==1&&finger_rcv_msg[4]==0x08)//�ȶԳ�ʱ
		{
			return ;
		}
	}
}

