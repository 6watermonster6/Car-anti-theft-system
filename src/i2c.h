#ifndef __I2C_H__
#define __I2C_H__

void HardI2CInit(void);

u8 HardI2CRecvByte(u8 word_addr);

void HardI2CSendByte(u8 word_addr,u8 data);

u8 HardI2CRecvBytes(u8 word_addr,u8 *buf,int count);

static u8 HardI2CPageProgramme(u8 word_addr,u8 *buf,int count);

u8 HardI2CSendBytes(u8 word_addr,u8 *buf,int count);

#endif