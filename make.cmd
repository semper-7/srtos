@echo off
setlocal enabledelayedexpansion
set PN=%cd%& cd ..
set PN=!PN:%cd%\=!& cd !PN!
set PATH=%PATH%;D:\ARM-GCC\bin\
set MCU=cortex-m3
set DEF=-DSTM32F103xB
set SF=stm32f10x.ld
set AS=arm-none-eabi-as
set CC=arm-none-eabi-gcc
set LD=arm-none-eabi-ld
set LS=arm-none-eabi-objdump
set OB=arm-none-eabi-objcopy
set SZ=arm-none-eabi-size
set AF=-mthumb -mcpu=%MCU%
set CF=-mthumb -mcpu=%MCU% -O2 -c -g %DEF% -Wall -std=gnu99 -ffunction-sections
set CI= -I inc
for /r inc /d %%I in (*) do set I=%%I& set CI=!CI! -I !I:%~dp0=!
set LF=--gc-sections

echo ******* Make proekt !PN! *******
md lst 2>nul
md obj 2>nul
del /q obj

for /r %%I in (*.s) do (
  echo Assembling %%~I ...
  %AS% %AF% -o obj\%%~nI.o %%~I
  if !errorlevel! NEQ 0 goto error
)

for /r %%I in (*.c) do (
  echo Compiling %%I ...
  %CC% %CF% %CI% -c %%I -o obj\%%~nI.o
  if !errorlevel! NEQ 0 goto error
)

echo Linking ...
for /r obj %%I in (*.o) do set O=!O! %%I
for /r lib %%I in (*.a) do set A=!A! %%I
%LD% %LF% -Map=lst\!PN!.map -T%SF% !O! !A! -o obj\!PN!.elf
if %errorlevel% NEQ 0 goto error

%LS% -S obj\!PN!.elf > lst\!PN!.lst
%OB% -O binary obj\!PN!.elf obj\!PN!.bin
%SZ% obj\!PN!.elf
:error
pause
