@echo off
setlocal enabledelayedexpansion
set P="C:\Program Files\STMicroelectronics\Software\Flash Loader Demo"
set COM=4
set DEVICE=STM32F1_Med-density_128K
set NP=%cd%& cd ..
set NP=!NP:%cd%\=!& cd !NP!
%P%\STMFlashLoader.exe -c --pn %COM% --br 115200 -i %DEVICE% -e --all -d --a 8000000 --fn !NP!.bin --v"
pause
