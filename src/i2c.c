#include "i2c.h"
#include "usart1.h"
#include "sysinit.h"

void i2cInit(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  // enable PORT_B
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  // --- GPIO setup ---
  //PB:6,7 - Alternate output open-drain 50MHz
  GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                 GPIO_CRL_MODE7 | GPIO_CRL_CNF7 );
  I2C1->CR1 = 0;
  I2C1->CR2 = SYSCLK / 2000000;
  I2C1->CCR = SYSCLK / 400000;
  I2C1->TRISE = SYSCLK / 2000000 + 1;
  I2C1->CR1 = I2C_CR1_PE;
}

//----------------------------------------------------------
uint8_t i2cRead(uint8_t i2c_addr, uint8_t addr, char* buf, uint16_t len)
{
  I2C1->CR1 = I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB));
  (void) I2C1->SR1;

  I2C1->DR = i2c_addr;
  while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)));
  if (I2C1->SR1 & I2C_SR1_AF) return 1;
  (void) I2C1->SR2;

  I2C1->DR = addr;
  while (!(I2C1->SR1 & I2C_SR1_TXE));

  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB));
  (void) I2C1->SR1;

  I2C1->DR=i2c_addr | 1;
  while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)));
  if (I2C1->SR1 & I2C_SR1_AF) return 1;
  (void) I2C1->SR2;

  for(int i=0;i<len;i++)
  {
    if(i == (len-1))
    {
      I2C1->CR1 &= ~I2C_CR1_ACK;
      I2C1->CR1 |= I2C_CR1_STOP;
    }
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    buf[i] = I2C1->DR;
  }
  return 0;
}

//----------------------------------------------------------
uint8_t i2cWrite(uint8_t i2c_addr, uint8_t addr, char* buf, uint16_t len)
{
  I2C1->CR1 = I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB));
  (void) I2C1->SR1;

  I2C1->DR = i2c_addr;
  while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)));
  if (I2C1->SR1 & I2C_SR1_AF) return 1;
  (void) I2C1->SR2;

  I2C1->DR = addr;
  while (!(I2C1->SR1 & I2C_SR1_TXE));

  for(int i=0;i<len;i++)
  {
    I2C1->DR = buf[i];
    while (!(I2C1->SR1 & I2C_SR1_TXE));
  }
  I2C1->CR1 |= I2C_CR1_STOP;
  return 0;
}

