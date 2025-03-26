#include <stm32f4xx.h>
#include "can.h"
#include "usart.h"
#include "led.h"

extern uint8_t can_receive_flag=0;
extern CanRxMsg can_rsvMsg={0};


/*--------------------------- CAN��Ϣ���� ---------------------------*/
// CAN������Ϣ�ṹ�壨��׼֡������֡��8�ֽڣ�
//data���趨{bit1:����ģ�飬bit2:���ֹ��ܣ�bit3:����ϸ��}
extern CanTxMsg Finger_info=		{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0,0,0,0,0,0,0xf5}};			// ָ����Ϣ֡(ʵ��������)
extern CanTxMsg Finger_getusernum=	{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0x01,0,0,0,0,0,0xf5}};		// ��ȡ�û�����
extern CanTxMsg Finger_user0add=	{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0x02,0x00,0,0,0,0,0xf5}};	// ����û�0
extern CanTxMsg Finger_user1add=	{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0x02,0x01,0,0,0,0,0xf5}};	// ����û�1
extern CanTxMsg Finger_user2add=	{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0x02,0x02,0,0,0,0,0xf5}};	// ����û�2
extern CanTxMsg Finger_deleteall=	{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0xdd,0xdd,0,0,0,0,0xf5}};	// ɾ�������û�
extern CanTxMsg Finger_verify=		{0x01,0,CAN_ID_STD,CAN_RTR_DATA,8,{0xf5,0x66,0x00,0,0,0,0,0xf5}};	// �ȶ�

void can1_init()
{
	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//�ṹ�嶨��
	CAN_InitTypeDef   CAN_InitStruct={0};
	GPIO_InitTypeDef  GPIO_InitStruct={0};
	NVIC_InitTypeDef  NVIC_InitStruct={0};
	CAN_FilterInitTypeDef CAN_FilterInitStruct={0};

	//can��ʼ������
	CAN_InitStruct.CAN_ABOM		=DISABLE;
	CAN_InitStruct.CAN_AWUM		=DISABLE;
	CAN_InitStruct.CAN_BS1		=CAN_BS1_4tq;
	CAN_InitStruct.CAN_BS2		=CAN_BS2_4tq;
	CAN_InitStruct.CAN_Mode		=CAN_Mode_Silent_LoopBack;
	CAN_InitStruct.CAN_NART		=DISABLE;
	CAN_InitStruct.CAN_Prescaler=8;
	CAN_InitStruct.CAN_RFLM		=DISABLE;
	CAN_InitStruct.CAN_SJW		=CAN_SJW_1tq;
	CAN_InitStruct.CAN_TTCM		=DISABLE;
	CAN_InitStruct.CAN_TXFP		=DISABLE;
	CAN_Init(CAN1,&CAN_InitStruct);

	//gpio��ʼ��
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	//�������Ÿ���Ϊcan1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1);

    NVIC_InitStruct.NVIC_IRQChannel=CAN1_RX0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

	//��ʼ��canɸѡ��
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment	=CAN_FIFO0;
	CAN_FilterInitStruct.CAN_FilterIdHigh			=0x0000;
	CAN_FilterInitStruct.CAN_FilterIdLow			=0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh		=0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow		=0x0002;
	CAN_FilterInitStruct.CAN_FilterMode				=CAN_FilterMode_IdMask;
	CAN_FilterInitStruct.CAN_FilterNumber			=0;
	CAN_FilterInitStruct.CAN_FilterScale			=CAN_FilterScale_32bit;
	CAN_FilterInitStruct.CAN_FilterActivation		=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStruct);

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
}

void CAN_SendData(CAN_TypeDef* CANx,CanTxMsg* CanData)
{
    uint8_t retrys=0;
    uint8_t mailbox=0;

    do
	{
		mailbox=CAN_Transmit(CANx,CanData);
		retrys++;
	}
	while((mailbox==CAN_TxStatus_NoMailBox)&&(retrys<0xFE));
	retrys=0;
}

void CAN1_RX0_IRQHandler(void)
{
    if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) // ����жϱ�־
    {
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);    // ����жϱ�־

		can_rsvMsg.StdId=0x00;
		can_rsvMsg.ExtId=0x00;
		can_rsvMsg.RTR=CAN_RTR_DATA;
		can_rsvMsg.IDE=CAN_ID_STD;
		can_rsvMsg.DLC=0;
		can_rsvMsg.FMI=0;
		can_rsvMsg.Data[0]=0x00;
		can_rsvMsg.Data[1]=0x00;
		can_rsvMsg.Data[2]=0x00;

        /* ��ȡ���յ�����Ϣ */
        CAN_Receive(CAN1, CAN_FIFO0, &can_rsvMsg);

		//led_ctrl(led_1,led_on);
		can_receive_flag=1;
		CAN_FIFORelease(CAN1,CAN_FIFO0);
    }
}



