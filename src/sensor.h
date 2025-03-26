#ifndef __SENSOR_H__
#define __SENSOR_H__

extern uint8_t rsv_msg[1024];
extern uint8_t rsv_index;
extern uint16_t flag;

void usart3_sensor_init();

void Usart3_SendByte(uint8_t data);

void Usart3_SendBytes(uint8_t *str,uint16_t n);

void GY39_RequestLight(void) ;

#endif