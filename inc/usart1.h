#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f1xx.h"
#define system_delay delay

void usartInit(uint32_t baud);
void usartWrite(uint8_t c);
void usartPrint(char *s);
void usartPrintNum(uint32_t n);
void usartTransmit(char *buffer, uint16_t buffer_size);
uint32_t usartReceive(char *buffer, uint16_t buffer_size);

#endif //__USART1_H__
