#ifndef __BT_H__
#define __BT_H__

extern uint8_t bt_rcv_msg[1024];	//usart3�������ݻ�����
extern uint8_t bt_rcv_count;			//usart3����ָ���
extern uint8_t bt_rcv_flag;			//һ��ָ�������ɱ�־λ��1��ʾ���

void bt_init(void);

void Usart3_SendByte(uint8_t data);

void Usart3_SendBytes(uint8_t *str,int n);



#endif
