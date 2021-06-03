#include "stm32f103xb.h"
#include "usart1.h"
#include "srtos.h"
#include "func.h"
#include "asmfunc.h"
#include <string.h>

#define USART_BUFFER_SIZE 80
char usart_rx_buffer[USART_BUFFER_SIZE];
char usart_tx_buffer[USART_BUFFER_SIZE];

void printReg(void)
{
  utoa(__get_CONTROL(), usart_tx_buffer, 1, HEX);
  usart_tx_buffer[1] = ' ';
  utoa(__get_SP(), usart_tx_buffer + 2, 8, HEX);
  usart_tx_buffer[10] = ' ';
  utoa(__get_LR(), usart_tx_buffer + 11, 8, HEX);
  usart_tx_buffer[19] = '\n';
  usart_tx_buffer[20] = 0;
  usartPrint(usart_tx_buffer);
}

void togleLed(void)
{
  GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void task0(void)
{
  while(1)
  {
    togleLed();
    delay(1000);
  }
}

void task1(void)
{
  while(1)
  {
    usartPrint("\n> ");
    /* Waiting receive command from Usart */ 
    usartReceive(usart_rx_buffer,USART_BUFFER_SIZE);
    /* Excluding \r and characters after */ 
    char* cr = strchr(usart_rx_buffer,'\r');
    if (cr) *(cr) = 0;
    void printReg();

    /* Parsing and command execution
    if      (!strcmp(usart_rx_buffer, "on led1" )) onLed1();
    else if (!strcmp(usart_rx_buffer, "off led1")) offLed1(); 
    else if (!strcmp(usart_rx_buffer, "on led2" )) led2 = 1;
    else if (!strcmp(usart_rx_buffer, "off led2")) led2 = 0;
    else if (!strcmp(usart_rx_buffer, "read i2c")) readI2c();
    else if (!strcmp(usart_rx_buffer, "write i2c")) writeI2c(); */
  }
}

void scanKey()
{
  volatile static uint32_t scan_old;
  volatile static uint32_t scan_changes;
  uint16_t scan = GPIOA->IDR & GPIO_IDR_IDR0;
  scan_changes = scan_old ^ scan;
  scan_old = scan;
  if (scan_changes & ~scan)
  {
    togleLed();
    usartPrint("Key\n");
  }
}

void gpioInit()
{
  // enable PORT_A, PORT_C
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);
  // --- GPIO setup ---
  //PA:0 - KEY input pull-up
  GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
  GPIOA->CRL |= GPIO_CRL_CNF0_1;
  //PA:0 = 1
  GPIOA->BSRR = GPIO_ODR_ODR0;
  //PC:13 - LED
  GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
}

int main()
{
  gpioInit();
  usartInit(115200);
  rtosInit();
  addTimer(scanKey,20,20,0);
  usartPrint("Start\n");
  addTask(task0);
  addTask(task1);
  rtosStart();
}
