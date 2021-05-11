#include "usart1.h"
#include "asmfunc.h"
#include "sysinit.h"
#include "srtos.h"

void usartWrite(uint8_t c) {
  while (DMA1_Channel4->CNDTR || !(USART1->SR & USART_SR_TXE));
  USART1->DR = c;
}

  // --- UART setup ----
void usartInit(uint32_t baud) {
  USART1->BRR = SYSCLK / baud;
  USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void usartPrint (char *s) {
  while (DMA1_Channel4->CNDTR || !(USART1->SR & USART_SR_TXE));
  DMA1_Channel4->CCR = 0;
  DMA1_Channel4->CPAR = (uint32_t)(&USART1->DR);
  DMA1_Channel4->CMAR = (uint32_t)s;
  DMA1_Channel4->CNDTR = _strlen(s);
  DMA1_Channel4->CCR = DMA_CCR1_MINC | DMA_CCR1_DIR;
  DMA1_Channel4->CCR |= DMA_CCR1_EN;
}

uint16_t usartReceive(char *bufer, uint16_t bufer_size) {
  int v;
  DMA1_Channel5->CCR = 0;
  DMA1_Channel5->CPAR = (uint32_t)(&USART1->DR);
  DMA1_Channel5->CMAR = (uint32_t)bufer;
  DMA1_Channel5->CNDTR = bufer_size;
  DMA1_Channel5->CCR = DMA_CCR1_MINC | DMA_CCR1_CIRC;
  DMA1_Channel5->CCR |= DMA_CCR1_EN;
  while (DMA1_Channel5->CNDTR == bufer_size);
  do {
    v = DMA1_Channel5->CNDTR;
    delay(20);
  } while (DMA1_Channel5->CNDTR != v);
  DMA1_Channel5->CCR = 0;
  int count = bufer_size - DMA1_Channel5->CNDTR;
  bufer[count]=0;
  return count;
}
