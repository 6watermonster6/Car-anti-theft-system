#ifndef __FINGERPRINT_H__
#define __FINGERPRINT_H__

extern uint8_t finger_rcv_msg[1024];	//usart6�������ݻ�����
extern uint8_t finger_rcv_count;			//usart6����ָ���
extern uint8_t finger_rcv_flag;			//һ��ָ�������ɱ�־λ��1��ʾ���
extern uint8_t finger_send_cmd[1024];	//��usart6������ָ��ģ���ָ��

typedef enum {USER0, USER1, USER2} user_num;//�û����룬��������û�

/*--------------------------- ָ��ģ��ָ� ---------------------------*/
// ��ָ֤�bit0��0xF5��        У��λ��bit6��bit1~bit5���
extern uint8_t Verify[8];		//�ȶ�1��N
extern uint8_t Add0_1[8];
extern uint8_t Add0_2[8];
extern uint8_t Add0_3[8];
extern uint8_t Add1_1[8];
extern uint8_t Add1_2[8];
extern uint8_t Add1_3[8];
extern uint8_t Add2_1[8];
extern uint8_t Add2_2[8];
extern uint8_t Add2_3[8];
extern uint8_t Add3_1[8];
extern uint8_t Add3_2[8];
extern uint8_t Add3_3[8];
extern uint8_t Add4_1[8];
extern uint8_t Add4_2[8];
extern uint8_t Add4_3[8];
extern uint8_t Delete_all[8];	// ɾ�������û�
extern uint8_t Get_count[8];		// ��ȡ�û�����

void Usart6_finger_init();

void Usart6_SendByte(uint8_t data);

void Usart6_SendBytes(uint8_t *str,uint16_t n);

uint8_t finger_usernum(void);

void finger_useradd(user_num user);

void finger_compere(void);

#endif