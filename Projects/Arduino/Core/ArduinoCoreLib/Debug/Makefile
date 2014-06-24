################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../../../../../Src/Core/CDC.cpp \
../../../../../Src/Core/HardwareSerial.cpp \
../../../../../Src/Core/HID.cpp \
../../../../../Src/Core/IPAddress.cpp \
../../../../../Src/Core/new.cpp \
../../../../../Src/Core/Print.cpp \
../../../../../Src/Core/Stream.cpp \
../../../../../Src/Core/Tone.cpp \
../../../../../Src/Core/USBCore.cpp \
../../../../../Src/Core/WInterrupts.c \
../../../../../Src/Core/wiring.c \
../../../../../Src/Core/wiring_analog.c \
../../../../../Src/Core/wiring_digital.c \
../../../../../Src/Core/wiring_pulse.c \
../../../../../Src/Core/wiring_shift.c \
../../../../../Src/Core/WMath.cpp \
../../../../../Src/Core/WString.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
CDC.o \
HardwareSerial.o \
HID.o \
IPAddress.o \
new.o \
Print.o \
Stream.o \
Tone.o \
USBCore.o \
WInterrupts.o \
wiring.o \
wiring_analog.o \
wiring_digital.o \
wiring_pulse.o \
wiring_shift.o \
WMath.o \
WString.o

OBJS_AS_ARGS +=  \
CDC.o \
HardwareSerial.o \
HID.o \
IPAddress.o \
new.o \
Print.o \
Stream.o \
Tone.o \
USBCore.o \
WInterrupts.o \
wiring.o \
wiring_analog.o \
wiring_digital.o \
wiring_pulse.o \
wiring_shift.o \
WMath.o \
WString.o

C_DEPS +=  \
CDC.d \
HardwareSerial.d \
HID.d \
IPAddress.d \
new.d \
Print.d \
Stream.d \
Tone.d \
USBCore.d \
WInterrupts.d \
wiring.d \
wiring_analog.d \
wiring_digital.d \
wiring_pulse.d \
wiring_shift.d \
WMath.d \
WString.d

C_DEPS_AS_ARGS +=  \
CDC.d \
HardwareSerial.d \
HID.d \
IPAddress.d \
new.d \
Print.d \
Stream.d \
Tone.d \
USBCore.d \
WInterrupts.d \
wiring.d \
wiring_analog.d \
wiring_digital.d \
wiring_pulse.d \
wiring_shift.d \
WMath.d \
WString.d

OUTPUT_FILE_PATH +=libArduinoCoreLib.a

OUTPUT_FILE_PATH_AS_ARGS +=libArduinoCoreLib.a

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler



































./%.o: ../../../../../Src/Core/%.c
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -DF_CPU=16000000L  -I"../../../../../src/core/variants/eightanaloginputs" -I"../../../../../src/core" -I"../../../../../src/core/variants/standard" -I"../Core"  -O0 -ffunction-sections -fdata-sections -mmcu=atmega328p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: ../../../../../Src/Core/%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -DF_CPU=16000000L  -I"../../../../../src/core/variants/eightanaloginputs" -I"../../../../../src/core" -I"../../../../../src/core/variants/standard" -I"../Core"  -O0 -ffunction-sections -fdata-sections -mmcu=atmega328p -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)


$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Archiver : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-ar.exe$(QUOTE) -r  -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) 
	@echo Finished building target: $@
	
	




# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "libArduinoCoreLib.elf" "libArduinoCoreLib.a" "libArduinoCoreLib.hex" "libArduinoCoreLib.lss" "libArduinoCoreLib.eep" "libArduinoCoreLib.map" "libArduinoCoreLib.srec" "libArduinoCoreLib.usersignatures"
	