#ifndef __ASMFUNC_H__
#define __ASMFUNC_H__

extern void _bzero(void *dest, uint8_t count);
extern void _memcpy(void *sour, void *dest, uint8_t count);
extern uint32_t _strlen(void *sour);
char *_stpcpy(char *dest, const char *sour);

#endif  // __ASMFUNC_H__
