#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

#include "stm32f1xx.h"

int ecmp(const char *s, const char *etalon);
uint32_t atou(const char *s);
char* atoip(const char *s, uint8_t *ip);

#endif //__ASMFUNC_H__
