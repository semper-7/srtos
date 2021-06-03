#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

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
    "	mov	lr,%0	" : : "J" (value) : "memory"
  );
}

inline void __SVC0(void)
{
  __asm volatile
  (
    "	svc	0	"
  );
}

inline uint32_t __push_r4_11(uint32_t value)
{
  __asm volatile
  (
    "	stmdb	%0!,{r4-r11}	" : "=r" (value) : "0" (value) : "memory"
  );
  return value;
}

inline uint32_t __pop_r4_11(uint32_t value)
{
  __asm volatile
  (
    "	ldmia	%0!,{r4-r11}	" : "=r" (value) : "0" (value) : "memory"
  );
  return value;
}

#endif  // __ASMFUNC_H__
