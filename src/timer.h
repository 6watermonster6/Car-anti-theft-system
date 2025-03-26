#ifndef __TIMER_H__
#define __TIMER_H__

typedef enum
{
	tim_on,
	tim_off
}timer_state;

void timer2_init(void);

void TIM2_IRQHandler(void);

void tim_pwm_init();

#endif