#include <stm32f4xx.h>
#include "i2c.h"
#include "delay.h"

void HardI2CInit(void)	//I2C�������ĳ�ʼ������
{
    //ʹ�ܿ�����ʱ�ӣ�����ʼ��I2C������
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Mode			=I2C_Mode_I2C;
	I2C_InitStruct.I2C_Ack			=I2C_Ack_Enable;
	I2C_InitStruct.I2C_ClockSpeed	=100000;
	I2C_InitStruct.I2C_DutyCycle	=I2C_DutyCycle_16_9;
	I2C_InitStruct.I2C_OwnAddress1	=0xFE;
	I2C_InitStruct.I2C_AcknowledgedAddress	=I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1,&I2C_InitStruct);
    
    //ʹ��GPIOʱ�ӣ�������GPIO���Ÿ��ù���ΪI2C���߽ӿ�
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
	
    //����I2C������
    I2C_Cmd(I2C1, ENABLE);
}

//ͨ��I2C�������� ���豸�н���1���ֽ�����
u8 HardI2CRecvByte(u8 slave_addr,u8 word_addr)
{
	// �����豸��ַ��дģʽ�������üĴ�����ַ
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1, word_addr);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	delay_ms(10);

	// �������л�����ģʽ
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	I2C_AcknowledgeConfig(I2C1, DISABLE); // ���һ���ֽڷ���NACK
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	u8 data = I2C_ReceiveData(I2C1);
	
	I2C_AcknowledgeConfig(I2C1, ENABLE); // �ָ�ACK
	return data;
}

//ͨ��I2C�������� ���豸����1���ֽ�����
void HardI2CSendByte(u8 word_addr,u8 data)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, word_addr << 1, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1, data);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

//ͨ��I2C�������� ���豸�н���n���ֽ�����
u8 HardI2CRecvBytes(u8 word_addr,u8 *buf,int count)
{
    if (count <= 0) return 0;
	// ���üĴ�����ַ
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, word_addr << 1, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// ������������������
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, word_addr << 1, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for (int i = 0; i < count; i++) {
		if (i == count - 1) {
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		buf[i] = I2C_ReceiveData(I2C1);
	}
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	return count;

}

//ҳ��̺������Լ�д���Լ��õĺ��������Բ����ҳ��
static u8 HardI2CPageProgramme(u8 word_addr,u8 *buf,int count)
{
    I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, word_addr << 1, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	for (int i = 0; i < count; i++) {
		I2C_SendData(I2C1, buf[i]);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	return count;

}
//ͨ��I2C�������� ���豸����n���ֽ����� => ���豸���ܴ��ڡ�ҳд������
u8 HardI2CSendBytes(u8 word_addr,u8 *buf,int count)
{
    int max_write = 0;		//�����ָ����ַ��ʼ���ܹ����д������ֽ�
	int can_write = 0;		//����ʵ����д���ֽ���
	int page_write = 0;		//���㵱ǰҳ��д���ֽ���
	int page_addr = 0;		//��ǰҳ����ʼ��ַ

	max_write = 256 - word_addr;
	can_write = count<max_write?count:max_write;
	max_write = can_write;		//��¼�û�ʵ��д�뵽AT24C02���ֽ���

	do{
		page_addr = word_addr & 0x07;		//�ֵ�ַ�ĵ�3bits��ҳ�ڵ�ַ
		page_write = 8 - page_addr;			//��ҳ�ڵ�ַ��ʼ����ǰҳ��д���ֽ���
		page_write = count<page_write?count:page_write;
		HardI2CPageProgramme(word_addr,buf,page_write);
		/* д��һ�κ����ʣ�µ�д��������д���ַ */
		count -= page_write;				//����ʣ��δд����ֽ���
		word_addr = word_addr + page_write;	//������һд��ĵ�ַ
		buf = buf+page_write;				//�����´�д��ʱ����������λ��
	}while(count > 0);		

	return max_write;
}

