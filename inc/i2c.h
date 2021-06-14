#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f103xb.h"

void i2cInit(void);
uint8_t i2cRead(uint8_t i2c_addr, uint8_t addr, char* buf, uint16_t len);
uint8_t i2cWrite(uint8_t i2c_addr, uint8_t addr, char* buf, uint16_t len);

#endif //__I2C_H__
