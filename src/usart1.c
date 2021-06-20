#include "usart1.h"
#include "sysinit.h"
#include <string.h>
#include "gpio.h"

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
  GPIOA_CRH( GP_M(9) | GP_M(10), GP_AOD50(9) | GP_IP(10) );
  //PA:10 = 1
  GPIOA->BSRR = GPIO_BSRR_BS10;

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
    system_delay(NEXT_BYTE_TIMEOUT);
  } while (DMA1_Channel5->CNDTR != v);

  DMA1_Channel5->CCR = 0;
  count = buffer_size - DMA1_Channel5->CNDTR;
  buffer[count]=0;
  return count;
}

char* utoa(uint32_t num, char *buffer)
{
  uint32_t quot;
  uint32_t tmp;
  uint8_t rem;
  char* s;
  s = buffer + 10;
  *s = 0;
    do
    {
      // "* 0.8"
      quot = num >> 1;
      quot += quot >> 1;
      quot += quot >> 4;
      quot += quot >> 8;
      quot += quot >> 16;
      tmp = quot;
      // "/ 8"
      quot >>= 3;
      rem = (uint8_t)(num - ((quot << 1) + (tmp & ~7ul)));
      if(rem > 9)
      {
        rem -= 10;
        quot++;
      }
      *--s = rem + '0';
      num = quot;
    } while (num);
  return s;
}

void usartPrintNum(uint32_t n)
{
  static char buffer[12];
  usartPrint(utoa(n, buffer));
}

