#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f1xx.h"
#define system_delay delay
#define LINE_TERMINAL        // wait end of line
#define NEXT_BYTE_TIMEOUT 20 // timeout waiting for the next byte in the packet

void usartInit(uint32_t baud);
void usartWrite(uint8_t c);
void usartPrint(char *s);
void usartPrintNum(uint32_t n);
void usartTransmit(char *buffer, uint16_t buffer_size);
uint32_t usartReceive(char *buffer, uint16_t buffer_size);
char* utoa(uint32_t num, char *buffer);

#endif //__USART1_H__
