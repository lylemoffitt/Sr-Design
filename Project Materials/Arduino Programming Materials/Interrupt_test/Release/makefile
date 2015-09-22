################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include arduino/core/subdir.mk
-include Libraries/TimerOne/subdir.mk
-include Libraries/Scheduler/src/subdir.mk
-include Libraries/SPI/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/arduino.ar \

ELF += \
Interrupt_test.elf \

EEP += \
Interrupt_test.eep \

FLASH_IMAGE += \
Interrupt_test.hex \


# All Target
all: Interrupt_test

# Tool invocations
Interrupt_test: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-size" -A "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.elf"
	@echo 'Finished building target: $@'
	@echo ' '

/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-ar" rcs  "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/arduino.ar"   $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

Interrupt_test.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-gcc" -Os -Wl,--gc-sections -mmcu=atmega328p  -o "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.elf"   $(LINK_OBJ) $(AR)  "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/arduino.ar" "-L/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release" -lm
	@echo 'Finished building: $@'
	@echo ' '

Interrupt_test.eep: $(ELF)
	@echo 'Create eeprom image'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-objcopy" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0  "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.elf" "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.eep"
	@echo 'Finished building: $@'
	@echo ' '

Interrupt_test.hex: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-objcopy" -O ihex -R .eeprom  "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.elf" "/Volumes/MacBook_HD/Developer/Eclipse-Workspaces/Arduino/Interrupt_test/Release/Interrupt_test.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_DEPS)$(AR)$(FLASH_IMAGE)$(EEP)$(CXX_DEPS)$(S_UPPER_DEPS)$(ELF)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(INO_DEPS)$(AR_OBJ)$(CPP_DEPS)$(LINK_OBJ)$(C_UPPER_DEPS)$(SIZEDUMMY) Interrupt_test
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
