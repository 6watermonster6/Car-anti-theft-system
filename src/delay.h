#ifndef __DELAY_H__
#define __DELAY_H__

void SysTick_Handler(void);

void delay(int n);

void delay_ms(int nms);

void delay_us(int nus);

#endif