#ifndef __LED_H__
#define __LED_H__

typedef enum
{
	led_on,
	led_off
}Led_state;

typedef enum
{
	led_0,
	led_1,
	led_2
}Led_num;

void led_init(void);

void led_ctrl(Led_num led,Led_state state);

#endif




