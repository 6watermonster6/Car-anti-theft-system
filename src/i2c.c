#include <stm32f4xx.h>
#include "i2c.h"
#include "delay.h"

void HardI2CInit(void)	//I2C控制器的初始化配置
{
    //使能控制器时钟，并初始化I2C控制器
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
    
    //使能GPIO时钟，并配置GPIO引脚复用功能为I2C总线接口
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin	=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode	=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	=GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd	=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
	
    //开启I2C控制器
    I2C_Cmd(I2C1, ENABLE);
}

//通过I2C控制器从 从设备中接收1个字节数据
u8 HardI2CRecvByte(u8 slave_addr,u8 word_addr)
{
	// 发送设备地址（写模式）以设置寄存器地址
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1, word_addr);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	delay_ms(10);

	// 启动并切换到读模式
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	I2C_AcknowledgeConfig(I2C1, DISABLE); // 最后一个字节发送NACK
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	u8 data = I2C_ReceiveData(I2C1);
	
	I2C_AcknowledgeConfig(I2C1, ENABLE); // 恢复ACK
	return data;
}

//通过I2C控制器向 从设备发送1个字节数据
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

//通过I2C控制器从 从设备中接收n个字节数据
u8 HardI2CRecvBytes(u8 word_addr,u8 *buf,int count)
{
    if (count <= 0) return 0;
	// 设置寄存器地址
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, word_addr << 1, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// 重新启动并接收数据
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

//页编程函数（自己写，自己用的函数，绝对不会跨页）
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
//通过I2C控制器向 从设备发送n个字节数据 => 从设备可能存在“页写”限制
u8 HardI2CSendBytes(u8 word_addr,u8 *buf,int count)
{
    int max_write = 0;		//计算从指定地址开始，能够最多写入多少字节
	int can_write = 0;		//计算实际能写的字节数
	int page_write = 0;		//计算当前页能写的字节数
	int page_addr = 0;		//当前页的起始地址

	max_write = 256 - word_addr;
	can_write = count<max_write?count:max_write;
	max_write = can_write;		//记录用户实际写入到AT24C02的字节数

	do{
		page_addr = word_addr & 0x07;		//字地址的低3bits是页内地址
		page_write = 8 - page_addr;			//从页内地址开始，当前页能写的字节数
		page_write = count<page_write?count:page_write;
		HardI2CPageProgramme(word_addr,buf,page_write);
		/* 写入一次后计算剩下的写入数量和写入地址 */
		count -= page_write;				//计算剩余未写入的字节数
		word_addr = word_addr + page_write;	//计算下一写入的地址
		buf = buf+page_write;				//计算下次写入时的数据所在位置
	}while(count > 0);		

	return max_write;
}

