#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"

//#define USE_USART_TXE_IRQ
//#define USART_RX_BUFER_SIZE 16

void usartInit(uint32_t baud);
void usartWrite(uint8_t c);
void usartPrint(char *s);
//void usartPrintDMA (char *s);
//uint8_t isUsartRead();
//uint8_t usartRead();
uint16_t usartReceive(char *bufer, uint16_t bufer_size);

#endif //__USART1_H__
