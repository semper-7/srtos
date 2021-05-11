/* Скрипт линковки */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
  ROM  (rx) : ORIGIN = 0x08000000, LENGTH = 64K  /* Объявляем регион ROM */
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K  /* Объявляем регион RAM */
}

PROVIDE(__stack_top__ = ORIGIN(RAM) + LENGTH(RAM)); /* __stack_top__ = конец RAM */
 
SECTIONS
{
  /* Секция .isr_vector, адрес: начало региона ROM */
  .isr_vector ORIGIN(ROM):
  {
    KEEP(*(.isr_vector))  /* все символы с атрибутом section(".isr_vector") */
  } >ROM

  /* Секция .text, выравнивание 16 байт */
  .text ALIGN(16):
  {
    *(.text*);   /* все символы с атрибутом section начинающимся со слова .text */
  } >ROM

  /* Секция констант, ASCI-строк в ROM*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)   /* содержит в ROM константы, ASCI-строки и т.п. */
  } >ROM

  /* Выходная секция для значений инициализации данных из .data */
  .data_rom ALIGN(4):
  {
    /* Создадим символ __data_rom__ с адресом равным текущему */
    PROVIDE(__data_rom__ = .);   
    /* Подвинем указатель на текущий адрес на размер выходной секции .data, она будет объявлена позже */
    . += SIZEOF(.data);     
  }

  /* Секция для неинициализированных переменных, адрес - начало региона RAM
     Тип NOLOAD, говорит компоновщику, что нужно только распределить адреса, данные нам не важны*/
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

  /* Секция .data, для инициализированных переменных, оператор AT() назначает LMA для этой секции */
  .data ALIGN(4): AT(__data_rom__)
  {
    PROVIDE(__data_ram__ = .);
    *(.data*)
  } >RAM
  PROVIDE(__data_len__ = . - __data_ram__);
 
}
