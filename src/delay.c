#include <stm32f4xx.h>
#include "delay.h"

static int time_out = 0;
//SysTick���жϴ�����(�ú�����stm32f4xx_it.c�ļ����Ѿ������壬�����Զ���ú���������ǰɾ��)
void SysTick_Handler(void)	//����SysTick��ʱ����ÿ�����趨��ʱ��ͻ�ִ��һ�α�����
{
    if(time_out > 0)
    {
        time_out--;
    }
}

void delay(int n)
{
    time_out = n;		//����SysTick���ж�ʱ���Ƕ��٣��Ϳ�����ʱ n���ж�ʱ��
    while(time_out);
}

void delay_ms(int nms)
{
    /* ����systick��ʱ����ÿ��1ms����һ���ж� */
    SysTick_Config(168000);
    /* �ȴ���ʱʱ�䵽�� */
    time_out = nms;
    while(time_out);
}

void delay_us(int nus)
{
    /* ����systick��ʱ����ÿ��1us����һ���ж� */
    SysTick_Config(168);
    /* �ȴ���ʱʱ�䵽�� */
    time_out = nus;		
    while(time_out);
}






