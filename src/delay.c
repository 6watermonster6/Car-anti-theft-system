#include <stm32f4xx.h>
#include "delay.h"

static int time_out = 0;
//SysTick的中断处理函数(该函数在stm32f4xx_it.c文件中已经被定义，如需自定义该函数，请提前删除)
void SysTick_Handler(void)	//配置SysTick定时器后，每个”设定“时间就会执行一次本函数
{
    if(time_out > 0)
    {
        time_out--;
    }
}

void delay(int n)
{
    time_out = n;		//设置SysTick的中断时间是多少，就可以延时 n个中断时间
    while(time_out);
}

void delay_ms(int nms)
{
    /* 配置systick定时器，每隔1ms产生一次中断 */
    SysTick_Config(168000);
    /* 等待延时时间到达 */
    time_out = nms;
    while(time_out);
}

void delay_us(int nus)
{
    /* 配置systick定时器，每隔1us产生一次中断 */
    SysTick_Config(168);
    /* 等待延时时间到达 */
    time_out = nus;		
    while(time_out);
}






