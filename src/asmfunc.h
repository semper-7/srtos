#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

extern void __bzero(void *dest, uint8_t count);
extern void __memcpy(void *sour, void *dest, uint8_t count);
uint32_t __strlen(void *sour);
char *__stpcpy(char *dest, const char *sour);

#endif  // __ASMFUNC_H__
