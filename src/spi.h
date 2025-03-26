#ifndef __SPI_H__
#define __SPI_H__

#define W25Q128_ENABLE()	GPIO_ResetBits(GPIOG,GPIO_Pin_8)	//拉低片选线
#define W25Q128_DISABLE()	GPIO_SetBits(GPIOG,GPIO_Pin_8)		//拉高片选线

/*
 *	My_SpiInit:W25Q128的通信接口SPI1
 *		SPI1_SCK	==>		PB3
 *		SPI1_MISO	==>		PB4
 *		SPI1_MOSI	==>		PB5
 *		F_CS		==>		PB14(W25Q128片选引脚)
 **/
void GEC_W25Q128Init(void);

void GEC_SPI1SendByte(uint16_t data);

uint16_t GEC_SPI1RecvByte(void);

//读取 制造商和芯片 ID ：华邦串行闪存 制造商ID：0xEF  芯片ID：0x17
uint16_t* W25Q128_ReadID(void);

//写使能
void W25Q128_WriteEnable(void);

//忙碌检测:读取控制和状态寄存器，并将busy返回（1忙碌，0空闲）
int W25Q128_GetBusyBit(void);

//读取数据
void W25Q128_ReadBytes(uint32_t word_addr,char buf[],int count);

//扇区擦除
void W25Q128_SectorErase(uint32_t sector_addr);
//页写：在调用函数前，就必须确保写入word_addr的count个字节数据不会超出当前页
//static void W25Q128_PageProgram(uint32_t word_addr,char buf[],int count);

//写入数据
//char beifen[4096]//用来保存备份数据
void W25Q128_WriteBytes(uint32_t word_addr,char buf[],int count);

//测试方法：参照at24c02的测试（写入数据，然后再读出来，打印到串口看是否跟写入的一样）

#endif