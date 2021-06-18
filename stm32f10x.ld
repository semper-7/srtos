/* Link script */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
  ROM  (rx) : ORIGIN = 0x08000000, LENGTH = 64K  /* ќбъ€вл€ем регион ROM */
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K  /* ќбъ€вл€ем регион RAM */
}

PROVIDE(__stack_top__ = ORIGIN(RAM) + LENGTH(RAM)); /* __stack_top__ = конец RAM */
 
SECTIONS
{
  .isr_vector ORIGIN(ROM):
  {
    KEEP(*(.isr_vector))
  } >ROM

  .text ALIGN(16):
  {
    *(.text*);
  } >ROM

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
  } >ROM

  .data_rom ALIGN(4):
  {
    PROVIDE(__data_rom__ = .);   
    . += SIZEOF(.data);     
  }

  .common ORIGIN(RAM) (NOLOAD) : 
  {
    *(COMMON*)                 
  }

  .bss ALIGN(4) (NOLOAD):
  {
    PROVIDE(__bss_ram__ = .);
    *(.bss*)
  }
  PROVIDE(__bss_len__ = . - __bss_ram__);

  .data ALIGN(4): AT(__data_rom__)
  {
    PROVIDE(__data_ram__ = .);
    *(.data*)
  } >RAM
  PROVIDE(__data_len__ = . - __data_ram__);
 
}
