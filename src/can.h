
#ifndef __CAN_H__
#define __CAN_H__

extern uint8_t can_receive_flag;
extern CanRxMsg can_rsvMsg;


/*--------------------------- CAN��Ϣ���� ---------------------------*/
// CAN������Ϣ�ṹ�壨��׼֡������֡��8�ֽڣ�
extern CanTxMsg Finger_info;		// ָ����Ϣ֡
extern CanTxMsg Finger_getusernum;	// ��ȡ�û�����
extern CanTxMsg Finger_user0add;	// ����û�
extern CanTxMsg Finger_user1add;	// ����û�
extern CanTxMsg Finger_user2add;	// ����û�
extern CanTxMsg Ready;			// ׼������

void can1_init();

void CAN_SendData(CAN_TypeDef* CANx,CanTxMsg* CanData);

#endif

