#ifndef __SPI_H__
#define __SPI_H__

#define W25Q128_ENABLE()	GPIO_ResetBits(GPIOG,GPIO_Pin_8)	//����Ƭѡ��
#define W25Q128_DISABLE()	GPIO_SetBits(GPIOG,GPIO_Pin_8)		//����Ƭѡ��

/*
 *	My_SpiInit:W25Q128��ͨ�Žӿ�SPI1
 *		SPI1_SCK	==>		PB3
 *		SPI1_MISO	==>		PB4
 *		SPI1_MOSI	==>		PB5
 *		F_CS		==>		PB14(W25Q128Ƭѡ����)
 **/
void GEC_W25Q128Init(void);

void GEC_SPI1SendByte(uint16_t data);

uint16_t GEC_SPI1RecvByte(void);

//��ȡ �����̺�оƬ ID ����������� ������ID��0xEF  оƬID��0x17
uint16_t* W25Q128_ReadID(void);

//дʹ��
void W25Q128_WriteEnable(void);

//æµ���:��ȡ���ƺ�״̬�Ĵ���������busy���أ�1æµ��0���У�
int W25Q128_GetBusyBit(void);

//��ȡ����
void W25Q128_ReadBytes(uint32_t word_addr,char buf[],int count);

//��������
void W25Q128_SectorErase(uint32_t sector_addr);
//ҳд���ڵ��ú���ǰ���ͱ���ȷ��д��word_addr��count���ֽ����ݲ��ᳬ����ǰҳ
//static void W25Q128_PageProgram(uint32_t word_addr,char buf[],int count);

//д������
//char beifen[4096]//�������汸������
void W25Q128_WriteBytes(uint32_t word_addr,char buf[],int count);

//���Է���������at24c02�Ĳ��ԣ�д�����ݣ�Ȼ���ٶ���������ӡ�����ڿ��Ƿ��д���һ����

#endif