################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/Wire/utility/twi.c 

C_DEPS += \
./Libraries/Wire/utility/twi.c.d 

LINK_OBJ += \
./Libraries/Wire/utility/twi.c.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Wire/utility/twi.c.o: /Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/Wire/utility/twi.c
	@echo 'Building file: $<'
	@echo 'Starting C compile'
	"/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc" -c -g -Os -ffunction-sections -fdata-sections -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=161 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/TimerOne" -I"/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/SPI" -I"/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/cores/arduino" -I"/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/variants/eightanaloginputs" -I"/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/fast_Digital" -I"/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/SPI_SRAM" -I"/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/Wire" -I"/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/Wire/utility" -I"/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/libraries/RF24" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"   -Wall  -std=gnu++11 -Wstack-usage=100 -feliminate-unused-debug-symbols -g0 -Os --param large-function-growth=100 --param large-stack-frame-growth=100 --param large-stack-frame=100 -fconserve-stack   -Wframe-larger-than=200 -fno-defer-pop -finline-functions --param large-function-insns=100
	@echo 'Finished building: $<'
	@echo ' '


