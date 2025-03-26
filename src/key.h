#ifndef __key_h__
#define __key_h__

void somedelay(void);

typedef enum
{
	key_0,
	key_1,
	key_2,
	key_3
}press_key;

void key_init(void);

press_key key_scan(void);

void KeyExtiInit(void);

void EXTI0_IRQHandler(void);


#endif
