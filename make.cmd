@echo off
setlocal enabledelayedexpansion
set NP=%cd%& cd ..
set NP=!NP:%cd%\=!& cd !NP!
echo ******* Make proekt !NP! *******
set PATH=%PATH%;D:\ARM-GCC\bin\
set MCU=cortex-m3
set DEF=-DSTM32F10X_MD -DSYSCLK_FREQ_72MHz
set CC=arm-none-eabi-gcc
set AS=arm-none-eabi-as
set CF=-mthumb -mcpu=%MCU% -O2 -c -g %DEF% -Wall -std=gnu99 -ffunction-sections
set AF=-mthumb -mcpu=%MCU%
set LF=--gc-sections
for /r inc /d %%I in (*) do set I=%%I& set CI=!CI! -I !I:%~dp0=!
set LD=arm-none-eabi-ld
set LS=arm-none-eabi-objdump
set OB=arm-none-eabi-objcopy
set SZ=arm-none-eabi-size

for /r %%I in (*.c) do set C=!C! %%I& echo Compile %%I ...
%CC% %CF% %CI% -c !C!
if %errorlevel% NEQ 0 goto error

for /r %%I in (*.asm) do echo Compile %%~I ...& %AS% %AF% -o %%~nI.o %%~I
if %errorlevel% NEQ 0 goto error

echo Linking ...
for %%I in (*.o) do set O=!O! %%I
%LD% %LF% -Map=!NP!.map -Tstm32f10x.ld !O! -o !NP!.elf
if %errorlevel% NEQ 0 goto error

%LS% -S !NP!.elf > !NP!.lst
%OB% -O binary !NP!.elf !NP!.bin
%SZ% !NP!.elf
:error
del *.o >nul
pause
