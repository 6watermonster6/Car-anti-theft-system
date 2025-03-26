#include <stm32f4xx.h>
#include "spi.h"

void typ_spi_init()
{
	SPI_InitTypeDef SPI_InitStruct={0};
	GPIO_InitTypeDef GPIO_InitStruct={0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	SPI_InitStruct.SPI_CPHA		=SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL		=SPI_CPOL_High;
	SPI_InitStruct.SPI_Mode		=SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS		=SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit	=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_DataSize	=SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_CRCPolynomial	=66;
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_Init(SPI1,&SPI_InitStruct);

	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);

	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_OUT;
	GPIO_Init(GPIOG,&GPIO_InitStruct);

	SPI_Cmd(SPI1, ENABLE);
}

void SPI1_SendByte(uint16_t data)
{
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)!=SET);
	
	SPI_SendData(SPI1,data);

	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)!=SET);
	
	SPI_ReceiveData(SPI1);
}

uint16_t SPI1_RecvByte(void)
{
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)!=SET);
	
	SPI_SendData(SPI1,0xff);

	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)!=SET);
	
	return SPI_ReceiveData(SPI1);
}

//读取 制造商和芯片 ID ：华邦串行闪存 制造商ID：0xEF  芯片ID：0x17
uint16_t* W25Q128_ReadID(void)
{
	uint16_t cmd[] = {0x90, 0x00, 0x00,0X00};
	SPI1_SendByte(cmd[0]);
	SPI1_SendByte(cmd[1]);
	SPI1_SendByte(cmd[2]);
	SPI1_SendByte(cmd[3]);
	uint16_t id[]={0};
	id[0]= SPI1_RecvByte();
	id[1]= SPI1_RecvByte();
	id[2]= SPI1_RecvByte();
	return id;
}

//写使能
void W25Q128_WriteEnable(void)
{

}

//忙碌检测:读取控制和状态寄存器，并将busy返回（1忙碌，0空闲）
int W25Q128_GetBusyBit(void)
{

}

//读取数据
void W25Q128_ReadBytes(uint32_t word_addr,char buf[],int count)
{

}
//扇区擦除
void W25Q128_SectorErase(uint32_t sector_addr)
{

}

//页写：在调用函数前，就必须确保写入word_addr的count个字节数据不会超出当前页
//static void W25Q128_PageProgram(uint32_t word_addr,char buf[],int count);

//写入数据
//char beifen[4096]//用来保存备份数据
void W25Q128_WriteBytes(uint32_t word_addr,char buf[],int count)
{

}

