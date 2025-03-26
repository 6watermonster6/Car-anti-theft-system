#ifndef __USART_H__
#define __USART_H__

void usart1_init();
void Usart1_SendByte(uint8_t data);
void Usart1_SendBytes(uint8_t str[],int n);
void USART1_IRQHandler(void);

#endif