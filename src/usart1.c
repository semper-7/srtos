#include "usart1.h"
#include "sysinit.h"
#include <string.h>

extern void system_delay(uint32_t time_ms);

void usartWrite(uint8_t c)
{
  while (DMA1_Channel4->CNDTR || !(USART1->SR & USART_SR_TXE));
  USART1->DR = c;
}

void usartInit(uint32_t baud)
{
// enable PORT_A, USART1
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN);
// enable DMA1
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
// --- GPIO setup ---
//PA:9  - Usart TX alternate output open drain 50MHz
//PA:10 - Usart RX input pull-up
  GPIOA->CRH &= ~(GPIO_CRH_CNF9   | GPIO_CRH_MODE9  |
                  GPIO_CRH_CNF10  | GPIO_CRH_MODE10);
  GPIOA->CRH |= (GPIO_CRH_CNF9    | GPIO_CRH_MODE9  |
                 GPIO_CRH_CNF10_1);
//PA:10 = 1
  GPIOA->BSRR = GPIO_ODR_ODR10;

  USART1->BRR = (uint16_t)(SYSCLK / baud);
  USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void usartPrint(char *s)
{
  usartTransmit(s, strlen(s));
}

void usartTransmit(char *buffer, uint16_t buffer_size)
{
  while (DMA1_Channel4->CNDTR || !(USART1->SR & USART_SR_TXE));
  DMA1_Channel4->CCR = 0;
  DMA1_Channel4->CPAR = (uint32_t)(&USART1->DR);
  DMA1_Channel4->CMAR = (uint32_t)buffer;
  DMA1_Channel4->CNDTR = buffer_size;
  DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_DIR;
  DMA1_Channel4->CCR |= DMA_CCR_EN;
}

uint32_t usartReceive(char *buffer, uint16_t buffer_size)
{
  uint32_t v;
  uint32_t count;
  DMA1_Channel5->CCR = 0;
  DMA1_Channel5->CPAR = (uint32_t)(&USART1->DR);
  DMA1_Channel5->CMAR = (uint32_t)buffer;
  DMA1_Channel5->CNDTR = buffer_size;
  DMA1_Channel5->CCR = DMA_CCR_MINC | DMA_CCR_CIRC;
  DMA1_Channel5->CCR |= DMA_CCR_EN;
  while (DMA1_Channel5->CNDTR == buffer_size);
  do
  {
    v = DMA1_Channel5->CNDTR;
  /* timeout waiting for the next byte in the packet */
    system_delay(20);
  } while (DMA1_Channel5->CNDTR != v);

  DMA1_Channel5->CCR = 0;
  count = buffer_size - DMA1_Channel5->CNDTR;
  buffer[count]=0;
  return count;
}
