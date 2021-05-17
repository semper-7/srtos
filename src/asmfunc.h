#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

void __bzero(void *dest, uint8_t count);
void __memcpy(void *sour, void *dest, uint8_t count);
uint32_t __strlen(void *sour);
char *__stpcpy(char *dest, const char *sour);

inline uint32_t __get_MEM(uint32_t addr)
{
  uint32_t value;
  __asm volatile
  (
    "	ldr	%0,[%1]	" : "=r" (value) :"r" (addr) : "memory"
  );
  return value;
}

inline uint32_t __get_SP(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mov	%0,sp	" : "=r" (value) : : "memory"
  );
  return value;
}

inline uint32_t __get_LR(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mov	%0,lr	" : "=r" (value) : : "memory"
  );
  return value;
}

inline uint32_t __get_PSP(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mrs	%0,psp	" : "=r" (value) : : "memory"
  );
  return value;
}

inline uint32_t __get_MSP(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mrs	%0,msp	" : "=r" (value) : : "memory"
  );
  return value;
}

inline uint32_t __get_CONTROL(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mrs	%0,control	" : "=r" (value) : : "memory"
  );
  return value;
}

inline void __set_MSP(uint32_t value)
{
  __asm volatile
  (
    "	msr	msp,%0	" : : "r" (value) : "memory"
  );
}

inline void __set_PSP(uint32_t value)
{
  __asm volatile
  (
    "	msr	psp,%0	" : : "r" (value) : "memory"
  );
}

inline void __set_CONTROL(uint32_t value)
{
  __asm volatile
  (
    "	msr	control,%0	" : : "r" (value) : "memory"
  );
}

inline void __set_BASEPRI(uint32_t value)
{
  __asm volatile
  (
    "	msr	basepri,%0	" : : "r" (value) : "memory"
  );
}

#endif  // __ASMFUNC_H__
