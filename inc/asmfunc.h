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

inline void __set_LR(uint32_t value)
{
  __asm volatile
  (
    "	mov	lr,%0	" : : "r" (value) : "memory"
  );
}

inline void __SVC0(void)
{
  __asm volatile
  (
    "	svc	0	"
  );
}


#endif  // __ASMFUNC_H__
