@echo off
"C:\Program Files (x86)\Arduino\hardware/tools/avr/bin/avrdude" -C"C:\Program Files (x86)\Arduino\hardware/tools/avr/etc/avrdude.conf" -v -v -v -v -patmega328p -carduino -P\\.\COM13 -b115200 -D -Uflash:w:C:\Embedded\Src\Tools\MPU6050\MPU6050Calib.hex:i

