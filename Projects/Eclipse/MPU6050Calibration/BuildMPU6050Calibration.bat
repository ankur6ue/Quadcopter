
@echo off

set CPPFileNames=MPU6050,..\..\libraries\I2Cdev\I2CDev,..\..\libraries\Wire\Wire
set CFileNames=..\..\libraries\Wire\Utility\twi

"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-g++" -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 -I"C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino" -I"C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard" -I"C:\Program Files (x86)\Arduino\libraries\I2Cdev" -I"C:\Program Files (x86)\Arduino\libraries\MPU6050"  -I"C:\Program Files (x86)\Arduino\libraries\Wire\Utility" -I"C:\Program Files (x86)\Arduino\libraries\Wire" "C:\Embedded\Src\Tools\MPU6050\MPU6050_calibration.cpp" -o C:\Embedded\Src\Tools\MPU6050\MPU6050_calibration.o 

FOR %%a IN (%CPPFileNames%) DO (
	echo Compiling %%a
   "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-g++" -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 -I"C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino" -I"C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard" -I"C:\Program Files (x86)\Arduino\libraries\I2Cdev" -I"C:\Program Files (x86)\Arduino\libraries\MPU6050"  -I"C:\Program Files (x86)\Arduino\libraries\Wire\Utility" -I"C:\Program Files (x86)\Arduino\libraries\Wire" "C:\Embedded\Src\Tools\MPU6050\%%a.cpp" -o C:\Embedded\Src\Tools\MPU6050\%%a.o 
)

FOR %%a IN (%CFileNames%) DO (
	echo Compiling %%a
   "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-gcc" -c -g -Os -Wall -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 -I"C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino" -I"C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard" -I"C:\Program Files (x86)\Arduino\libraries\I2Cdev" -I"C:\Program Files (x86)\Arduino\libraries\MPU6050"  -I"C:\Program Files (x86)\Arduino\libraries\Wire\Utility" "C:\Program Files (x86)\Arduino\libraries\MPU6050\%%a.c" -o C:\Embedded\Src\Tools\MPU6050\%%a.o 
)

echo making elf file

setlocal enabledelayedexpansion 
set ObjFiles=
FOR %%a IN (%CPPFileNames%) DO (
	Set ObjFiles=!ObjFiles! C:\Embedded\Src\Tools\MPU6050\%%a.o
)

FOR %%a IN (%CFileNames%) DO (
	Set ObjFiles=!ObjFiles! C:\Embedded\Src\Tools\MPU6050\%%a.o
)

"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-gcc" -Os -Wl,--gc-sections -mmcu=atmega328p -o C:\Embedded\Src\Tools\MPU6050\MPU6050Calib.elf C:\Embedded\Src\Tools\MPU6050\MPU6050_calibration.o %ObjFiles% C:\Embedded\Bin\AVRCore\core.a -lm 

echo making hex file

"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-objcopy" -O ihex -R .eeprom C:\Embedded\Src\Tools\MPU6050\MPU6050Calib.elf C:\Embedded\Src\Tools\MPU6050\MPU6050Calib.hex 