#ifndef __FINGERPRINT_H__
#define __FINGERPRINT_H__

extern uint8_t finger_rcv_msg[1024];	//usart6接收数据缓冲区
extern uint8_t finger_rcv_count;			//usart6接收指令长度
extern uint8_t finger_rcv_flag;			//一条指令存贮完成标志位：1表示完成
extern uint8_t finger_send_cmd[1024];	//从usart6发出给指纹模块的指令

typedef enum {USER0, USER1, USER2} user_num;//用户号码，用于添加用户

/*--------------------------- 指纹模块指令集 ---------------------------*/
// 验证指令：bit0（0xF5）        校验位：bit6（bit1~bit5异或）
extern uint8_t Verify[8];		//比对1：N
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
extern uint8_t Delete_all[8];	// 删除所有用户
extern uint8_t Get_count[8];		// 获取用户数量

void Usart6_finger_init();

void Usart6_SendByte(uint8_t data);

void Usart6_SendBytes(uint8_t *str,uint16_t n);

uint8_t finger_usernum(void);

void finger_useradd(user_num user);

void finger_compere(void);

#endif