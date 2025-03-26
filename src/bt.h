#ifndef __BT_H__
#define __BT_H__

extern uint8_t bt_rcv_msg[1024];	//usart3接收数据缓冲区
extern uint8_t bt_rcv_count;			//usart3接收指令长度
extern uint8_t bt_rcv_flag;			//一条指令存贮完成标志位：1表示完成

void bt_init(void);

void Usart3_SendByte(uint8_t data);

void Usart3_SendBytes(uint8_t *str,int n);



#endif
