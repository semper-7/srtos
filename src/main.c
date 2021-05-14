#include "stm32f10x.h"
#include "usart1.h"
#include "srtos.h"
#include "func.h"
#include "asmfunc.h"

#define USART_BUFER_SIZE 80
char usart_rx_bufer[USART_BUFER_SIZE];
char usart_tx_bufer[USART_BUFER_SIZE];

void task0(void) {
  usartPrint("Start task0\n");
  while(1);
}


void togleLed(void) {
  GPIOC->ODR ^= GPIO_ODR_ODR13;
  char *s = itoa(usart_tx_bufer, __get_MSP(), 8, HEX);
  *(s++) = '\t';
  s = itoa(s, __get_PSP(), 8, HEX);
  *(s++) = '\t';
  s = itoa(s, __get_CONTROL(), 1, HEX);
  *(s++) = '\n';
  *(s++) = 0;
  usartPrint(usart_tx_bufer);
}

void scanKey() {
  volatile static uint32_t scan_old;
  volatile static uint32_t scan_changes;
  uint16_t scan = GPIOA->IDR & GPIO_IDR_IDR0;
  scan_changes = scan_old ^ scan;
  scan_old = scan;
  if (scan_changes & ~scan) {
    togleLed();
    usartPrint("Key\n");
  }
}

int main()
{
// enable PORT_A, PORT_C, USART1
  RCC->APB2ENR |= (RCC_APB2RSTR_IOPARST    | RCC_APB2RSTR_IOPCRST |
                   RCC_APB2RSTR_USART1RST) ;
// enable DMA1
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
// --- GPIO setup ---
//PA:0 - KEY input pull-up
  GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
  GPIOA->CRL |= GPIO_CRL_CNF0_1;
//PA:9  - Usart TX alternate output open drain 50MHz
//PA:10 - Usart RX input pull-up
  GPIOA->CRH &= ~(GPIO_CRH_CNF9   | GPIO_CRH_MODE9  |
                  GPIO_CRH_CNF10  | GPIO_CRH_MODE10);
  GPIOA->CRH |= (GPIO_CRH_CNF9    | GPIO_CRH_MODE9  |
                 GPIO_CRH_CNF10_1);
//PA:0,10 = 1
  GPIOA->BSRR = GPIO_ODR_ODR0 | GPIO_ODR_ODR10;
//PC:13 - LED
  GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
  GPIOC->CRH |= GPIO_CRH_MODE13_1;

  usartInit(115200);
  rtosInit(0);
  addTimer(scanKey,20,20);
  addTimer(togleLed,1000,1000);
  usartPrint("Start\n");
  usartPrint("Begin\n");
  char *s = itoa(usart_tx_bufer, __get_CONTROL(), 1, HEX);
  *(s++) = '\n';
  *(s++) = 0;
  usartPrint(usart_tx_bufer);
  addTask(task0);
  __start_RTOS(task0);

//    delay(1000);

}

