#include "stm32f1xx.h"
#include "usart1.h"
#include "i2c.h"
#include "srtos.h"
#include <string.h>
#include "asmfunc.h"
#include "gpio.h"
#include "network.h"

#define USART_BUFFER_SIZE 80
char usart_rx_buffer[USART_BUFFER_SIZE];
char i2cbuf[256];

static uint8_t led0;
static uint8_t led1;

byte ipaddr[4] = {192,168,1,111};
byte ipgw[4] = {192,168,1,1};
byte ipdns[4] = {212,94,96,123};

void timer2Init()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->CR1 = 0;
  TIM2->ARR = 65535;
  /* Mode PWM1 */
  TIM2->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  /* Channel 1 output active low */
  TIM2->CCER = TIM_CCER_CC1P | TIM_CCER_CC1E;
  TIM2->CR1 = TIM_CR1_CEN;
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

void taskBlink(void)
{
  while(led0)
  {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
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
  usartPrint("Id	Name	Stat\n");
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

void killTask(void)
{
  uint8_t id;
  if (usart_rx_buffer[5] <= '9')
  {
    id = atou(&usart_rx_buffer[5]);
  }
  else
  {
    id = getTaskId(&usart_rx_buffer[5]);
  }

  if (id && id<TSK)
  {
    removeTask(id);
    usartPrint("Task ");
    usartPrint(&usart_rx_buffer[5]);
    usartPrint(" stopped");
  }
}

void etherLink(void)
{
  static uint8_t link;
  uint8_t tmp;
  while (1)
  {
    tmp = LinkFunc();
    if (tmp != link) 
    {
      link = tmp;
      if (tmp) usartPrint("Link UP\n");
      else     usartPrint("Link Down\n");
    }
    delay(1000);
  }
}

void net(void)
{
  while(1) PacketFunc();
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
    if      (!strcmp(usart_rx_buffer, "ps"       )) statTask();
    else if (  !ecmp(usart_rx_buffer, "kill "    )) killTask();
    else if (!strcmp(usart_rx_buffer, "on led0"  )) onLed0();
    else if (!strcmp(usart_rx_buffer, "off led0" )) led0 = 0;
    else if (!strcmp(usart_rx_buffer, "on led1"  )) led1 = 1;
    else if (!strcmp(usart_rx_buffer, "off led1" )) led1 = 0;
    else if (!strcmp(usart_rx_buffer, "read i2c" )) readI2c();
    else if (!strcmp(usart_rx_buffer, "write i2c")) writeI2c();
  }
}

void scanKey()
{
  volatile static uint32_t scan_old;
  volatile static uint32_t scan_changes;

  while(1)
  {
    uint16_t scan = GPIOA->IDR & GPIO_IDR_IDR1;
    scan_changes = scan_old ^ scan;
    scan_old = scan;
    if (scan_changes & ~scan) led1 ^= 1;
    delay(40);
  }
}

void gpioInit()
{
  // enable PORT_A, PORT_C
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);
  // --- GPIO setup ---
  GPIOA_CRL( GP_M(0) | GP_M(1), GP_APP2(0) | GP_IP(1) );
  GPIOC_CRH( GP_M(13), GP_PP2(13) );
  GPIOA->BSRR = GPIO_BSRR_BS1;
  GPIOC->BSRR = GPIO_BSRR_BS13;
}

int main()
{
  gpioInit();
  timer2Init();
  i2cInit();
  usartInit(115200);
  usartPrint("Start SRTOS\n");
  ENC28J60_Init();
  addTask("scanKey", scanKey, 20);
  addTask("CLI", taskCLI, 0);
  addTask("net", net, 0);
  addTask("link", etherLink, 0);
  startRtos();
}

void SysTickCallback(void)
{
  static volatile uint16_t pwm;
  if (led1)
  {
    if (pwm<65280) pwm+=((pwm>>8) + 1); /* led1 pwm on */
  }
  else
  {
    if (pwm>0) pwm-=((pwm>>8) + 1); /* led1 pwm off */
  }
  TIM2->CCR1 = pwm;
}
