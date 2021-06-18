#include "stm32f1xx.h"
#include "usart1.h"
#include "i2c.h"
#include "srtos.h"
#include <string.h>

#define USART_BUFFER_SIZE 80
char usart_rx_buffer[USART_BUFFER_SIZE];
char usart_tx_buffer[USART_BUFFER_SIZE];

char i2cbuf[256];

uint8_t led0 = 0;
uint8_t led2 = 0;

void onLed1(void)
{
  GPIOA->BRR = GPIO_BRR_BR5;
}

void offLed1(void)
{
  GPIOA->BSRR = GPIO_BSRR_BS5;
}

void timer3Init()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3->CR1 = 0;
  TIM3->ARR = 65535;
  /* Channel 1 mode PWM1 */
  TIM3->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  /* Output active low */
  TIM3->CCER = TIM_CCER_CC1P | TIM_CCER_CC1E;
  TIM3->CR1 = TIM_CR1_CEN;
}

void readI2c()
{
 if (!i2cRead(0xA0, 0, i2cbuf, 256))
 {
   usartTransmit(i2cbuf, 256);
 }
 else
 {
   usartPrint("Error i2c");
 }
}

void writeI2c()
{
 if (i2cWrite(0xA0, 16, "test!!!", 8)) usartPrint("Error i2c");
}

void togleLed(void)
{
  GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void taskBlink(void)
{
  while(led0)
  {
    togleLed();
    delay(300);
  }
  GPIOC->BSRR = GPIO_BSRR_BS13;
}

void onLed0(void)
{
  if (!led0)
  {
    led0 = 1;
    addTask("Blink", taskBlink, 0);
  }
}

void statTask(void)
{
  for (int i=0; i<TSK; i++)
  {
    if (isTask(i))
    {
      usartPrintNum(i);
      usartWrite('\t');
      usartPrint(getTaskName(i));
      usartWrite('\t');
      usartPrintNum(getTaskStat(i));
      usartWrite('\n');
    }
  }
}

void taskCLI(void)
{
  while(1)
  {
    usartPrint("\n> ");
    /* Waiting receive command from Usart */ 
    usartReceive(usart_rx_buffer,USART_BUFFER_SIZE);
    /* Excluding \r and characters after */ 
    char* cr = strchr(usart_rx_buffer,'\r');
    if (cr) *(cr) = 0;
    /* Parsing and command execution */
    if      (!strcmp(usart_rx_buffer, "on led0" )) onLed0();
    else if (!strcmp(usart_rx_buffer, "off led0")) led0 = 0;
    else if (!strcmp(usart_rx_buffer, "on led1" )) onLed1();
    else if (!strcmp(usart_rx_buffer, "off led1")) offLed1(); 
    else if (!strcmp(usart_rx_buffer, "on led2" )) led2 = 1;
    else if (!strcmp(usart_rx_buffer, "off led2")) led2 = 0;
    else if (!strcmp(usart_rx_buffer, "read i2c")) readI2c();
    else if (!strcmp(usart_rx_buffer, "write i2c")) writeI2c();
    else if (!strcmp(usart_rx_buffer, "stat")) statTask();
  }
}

void scanKey()
{
  volatile static uint32_t scan_old;
  volatile static uint32_t scan_changes;

  while(1)
  {
    uint16_t scan = GPIOA->IDR & GPIO_IDR_IDR0;
    scan_changes = scan_old ^ scan;
    scan_old = scan;
    if (scan_changes & ~scan)
    {
      togleLed();
      usartPrint("Key\n");
    }
    delay(20);
  }
}

void gpioInit()
{
  // enable PORT_A, PORT_C
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);
  // --- GPIO setup ---
  GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 |
                  GPIO_CRL_CNF5 | GPIO_CRL_MODE5 |
                  GPIO_CRL_CNF6 | GPIO_CRL_MODE6 );
  //PA:0 - KEY input pull-up
  GPIOA->CRL |= (GPIO_CRL_CNF0_1 |
  //PA:5 - LED1, output push-pull 2MHz
                GPIO_CRL_MODE5_1 |
  //PA:6 - LED2, alternate output push-pull 2MHz
                GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1 );
  //PA:0 = 1
  GPIOA->BSRR = GPIO_BSRR_BS0 |
  		GPIO_BSRR_BS5 | GPIO_BSRR_BS6;
  //PC:13 - LED0, output push-pull 2MHz
  GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
  GPIOC->BSRR = GPIO_BSRR_BS13;
}

int main()
{
  gpioInit();
  usartInit(115200);
  usartPrint("Start SRTOS\n");
  timer3Init();
  i2cInit();
  addTask("scanKey", scanKey, 20);
  addTask("CLI", taskCLI, 0);
  startRtos();
}

void SysTickCallback(void)
{
  static volatile uint16_t pwm;
  if (led2)
  {
    if (pwm<65280) pwm+=((pwm>>8) + 1); /* led2 on */
  }
  else
  {
    if (pwm>0) pwm-=((pwm>>8) + 1); /* led2 off */
  }
  TIM3->CCR1 = pwm;
}
