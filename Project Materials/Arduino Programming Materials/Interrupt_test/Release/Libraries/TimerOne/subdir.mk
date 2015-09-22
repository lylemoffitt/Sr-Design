################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/TimerOne/TimerOne.cpp 

CPP_DEPS += \
./Libraries/TimerOne/TimerOne.cpp.d 

LINK_OBJ += \
./Libraries/TimerOne/TimerOne.cpp.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/TimerOne/TimerOne.cpp.o: /Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/TimerOne/TimerOne.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=158 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino" -I"/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/variants/eightanaloginputs" -I"/Applications/Arduino.app/Contents/Java/libraries/Scheduler" -I"/Applications/Arduino.app/Contents/Java/libraries/Scheduler/src" -I"/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/TimerOne" -I"/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/libraries/SPI" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


