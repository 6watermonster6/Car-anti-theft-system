/*--------------------------- ͷ�ļ����� ---------------------------*/
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
#include "spi.h"
#include "fingerprint.h"
#include "can.h"
#include "bt.h"

/*--------------------------- �������� ---------------------------*/
void all_init();				//��ʼ��ȫ��
void print_rsv_bit(void);		//�򴮿�1��ӡָ��ģ�鷢�ͻ�������Ϣ
void key_on_finger(void);		//��������ָ��
void can_to_finger(void);		//can���߿���ָ��

/*--------------------------- ȫ�ֱ������� ---------------------------*/
/*
extern uint8_t finger_rcv_msg[1024]={0};	//USART3�������ݻ�����
extern uint8_t finger_rcv_count=0;			//USART3����ָ���
extern uint8_t finger_rcv_flag=0;			//һ��ָ�������ɱ�־λ��1��ʾ���


/*------------------------------- ������ -------------------------------*/
int main()
{		
	all_init();//��ʼ��һ��

    // ��ѭ��
	//key_on_finger();	//���Ժ���
	//can_to_finger();	//���Ժ���
	while(1)
	{
		//�ж�can�����Ƿ��յ���Ϣ(�յ�Ϊ1��û�յ�Ϊ0)
		if(can_receive_flag==1){
			can_receive_flag=0;//���ñ�־λ
			
			//״̬������can�յ�����Ϣ
			if(can_rsvMsg.Data[0]==0xf5)//���ݶ����ֽ�Ϊ0xf5Ϊָ��ģ����Ϣ
			{
				switch (can_rsvMsg.Data[1]) {
					case 0x01: // ��ȡ�û�����ָ��
						printf("finger reader's number is %d",finger_usernum());
						//print_rsv_bit();
						break;
					case 0x02: 	// ����û�ָ��
						if(can_rsvMsg.Data[2]==0x00)finger_useradd(USER0);
						else if(can_rsvMsg.Data[2]==0x01)finger_useradd(USER1);
						else if(can_rsvMsg.Data[2]==0x02)finger_useradd(USER2);
						break;
					case 0xdd: 	// ɾ���û�ָ��
						Usart6_SendBytes(Delete_all,8);
						print_rsv_bit();
						break;
					case 0x66:	//�ȶ�
						finger_compere();
					default:
						// δʶ���ָ�����״̬
						
						break;
				}
			}
		}
	}
	return 0;
}

void all_init()
{
	key_init();
	led_init();
	//KeyExtiInit();
	usart1_init();
	//timer2_init();
	usart6_finger_init();
	bt_init();
	can1_init();
}

void print_rsv_bit(void)
{
	for(int i=0;i<8;i++)
	{
		printf("%x ",finger_rcv_msg[i]);
	}
	printf("\n");
}

void key_on_finger(void)
{
    while (1) 
	{
		press_key KEY=key_scan();
		// ��֧����ָͬ��
		switch (KEY) {
			case key_0:	// ��ȡ�û�����ָ��
				//printf("finger reader's number is %d",finger_usernum());
				finger_compere();
				print_rsv_bit();
				break;
			case key_2:	// ����û�ָ��
				finger_useradd(USER1);
				break;
			case key_3:	// ɾ���û�ָ��
				Usart6_SendBytes(Delete_all,8);
				print_rsv_bit();
				break;
			default:
				// δʶ���ָ�����״̬
				
				break;
		}
    }
}

void can_to_finger(void)
{
	CAN_SendData(CAN1,&Finger_user0add);
	while(1)
	{
		//CAN_SendData(CAN1,&Finger_getusernum);
		if(can_receive_flag==1){
			can_receive_flag=0;
			if(can_rsvMsg.Data[0]==0xf5){
				switch (can_rsvMsg.Data[1]) {
					case 0x01: // ��ȡ�û�����ָ��
						led_ctrl(led_0,led_on);
						printf("finger reader's number is %d",finger_usernum());
						delay_ms(500);
						//finger_compere();
						//print_rsv_bit();
						break;
					case 0x02: // ����û�ָ��
						Usart6_SendBytes(Delete_all,8);
						delay_ms(500);
						finger_useradd(USER0);
						break;
					case 0x09: // ɾ���û�ָ��
						Usart6_SendBytes(Delete_all,8);
						print_rsv_bit();
						break;
					default:
						// δʶ���ָ�����״̬
						
						break;
				}
			}
		}
	}
}

