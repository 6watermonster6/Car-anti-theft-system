
#ifndef __CAN_H__
#define __CAN_H__

extern uint8_t can_receive_flag;
extern CanRxMsg can_rsvMsg;


/*--------------------------- CAN消息定义 ---------------------------*/
// CAN发送消息结构体（标准帧，数据帧，8字节）
extern CanTxMsg Finger_info;		// 指纹信息帧
extern CanTxMsg Finger_getusernum;	// 获取用户数量
extern CanTxMsg Finger_user0add;	// 添加用户
extern CanTxMsg Finger_user1add;	// 添加用户
extern CanTxMsg Finger_user2add;	// 添加用户
extern CanTxMsg Ready;			// 准备就绪

void can1_init();

void CAN_SendData(CAN_TypeDef* CANx,CanTxMsg* CanData);

#endif

