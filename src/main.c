/*--------------------------- 头文件部分 ---------------------------*/
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

/*--------------------------- 函数声明 ---------------------------*/
void all_init();				//初始化全部
void print_rsv_bit(void);		//向串口1打印指纹模块发送回来的消息
void key_on_finger(void);		//按键控制指纹
void can_to_finger(void);		//can总线控制指纹

/*--------------------------- 全局变量定义 ---------------------------*/
/*
extern uint8_t finger_rcv_msg[1024]={0};	//USART3接收数据缓冲区
extern uint8_t finger_rcv_count=0;			//USART3接收指令长度
extern uint8_t finger_rcv_flag=0;			//一条指令存贮完成标志位：1表示完成


/*------------------------------- 主函数 -------------------------------*/
int main()
{		
	all_init();//初始化一切

    // 主循环
	//key_on_finger();	//测试函数
	//can_to_finger();	//测试函数
	while(1)
	{
		//判断can总线是否收到消息(收到为1，没收到为0)
		if(can_receive_flag==1){
			can_receive_flag=0;//重置标志位
			
			//状态机处理can收到的消息
			if(can_rsvMsg.Data[0]==0xf5)//数据段首字节为0xf5为指纹模块消息
			{
				switch (can_rsvMsg.Data[1]) {
					case 0x01: // 获取用户数量指令
						printf("finger reader's number is %d",finger_usernum());
						//print_rsv_bit();
						break;
					case 0x02: 	// 添加用户指令
						if(can_rsvMsg.Data[2]==0x00)finger_useradd(USER0);
						else if(can_rsvMsg.Data[2]==0x01)finger_useradd(USER1);
						else if(can_rsvMsg.Data[2]==0x02)finger_useradd(USER2);
						break;
					case 0xdd: 	// 删除用户指令
						Usart6_SendBytes(Delete_all,8);
						print_rsv_bit();
						break;
					case 0x66:	//比对
						finger_compere();
					default:
						// 未识别的指令，重置状态
						
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
		// 分支处理不同指令
		switch (KEY) {
			case key_0:	// 获取用户数量指令
				//printf("finger reader's number is %d",finger_usernum());
				finger_compere();
				print_rsv_bit();
				break;
			case key_2:	// 添加用户指令
				finger_useradd(USER1);
				break;
			case key_3:	// 删除用户指令
				Usart6_SendBytes(Delete_all,8);
				print_rsv_bit();
				break;
			default:
				// 未识别的指令，重置状态
				
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
					case 0x01: // 获取用户数量指令
						led_ctrl(led_0,led_on);
						printf("finger reader's number is %d",finger_usernum());
						delay_ms(500);
						//finger_compere();
						//print_rsv_bit();
						break;
					case 0x02: // 添加用户指令
						Usart6_SendBytes(Delete_all,8);
						delay_ms(500);
						finger_useradd(USER0);
						break;
					case 0x09: // 删除用户指令
						Usart6_SendBytes(Delete_all,8);
						print_rsv_bit();
						break;
					default:
						// 未识别的指令，重置状态
						
						break;
				}
			}
		}
	}
}

