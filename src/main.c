#include "stm32f103xb.h"
#include "usart1.h"
#include "srtos.h"
#include "func.h"
#include "asmfunc.h"

#define USART_BUFER_SIZE 80
char usart_rx_bufer[USART_BUFER_SIZE];
char usart_tx_bufer[USART_BUFER_SIZE];

void task0(void)
{
  while(1)
  {
    usartPrint("task0\n");
    char *s = itoa(usart_tx_bufer, __get_CONTROL(), 1, HEX);
    *(s++) = ' ';
    s = itoa(s, __get_SP(), 8, HEX);
    *(s++) = '\n';
    *(s++) = 0;
    usartPrint(usart_tx_bufer);
    delay(2000);
  }
}

void togleLed(void)
{
  GPIOC->ODR ^= GPIO_ODR_ODR13;
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
  RCC->APB2ENR |= (RCC_APB2RSTR_IOPARST | RCC_APB2RSTR_IOPCRST);
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
  addTimer(togleLed,1000,1000,0);
  usartPrint("Start\n");
  addTask(task0);
  rtosStart();
}
