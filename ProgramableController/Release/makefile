################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include 1Wire/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
UNIWERSALNY_STEROWNIK.lss \

FLASH_IMAGE += \
UNIWERSALNY_STEROWNIK.hex \

EEPROM_IMAGE += \
UNIWERSALNY_STEROWNIK.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: UNIWERSALNY_STEROWNIK.elf secondary-outputs

# Tool invocations
UNIWERSALNY_STEROWNIK.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,UNIWERSALNY_STEROWNIK.map -mmcu=atmega644p -o"UNIWERSALNY_STEROWNIK.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

UNIWERSALNY_STEROWNIK.lss: UNIWERSALNY_STEROWNIK.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S UNIWERSALNY_STEROWNIK.elf  >"UNIWERSALNY_STEROWNIK.lss"
	@echo 'Finished building: $@'
	@echo ' '

UNIWERSALNY_STEROWNIK.hex: UNIWERSALNY_STEROWNIK.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex UNIWERSALNY_STEROWNIK.elf  "UNIWERSALNY_STEROWNIK.hex"
	@echo 'Finished building: $@'
	@echo ' '

UNIWERSALNY_STEROWNIK.eep: UNIWERSALNY_STEROWNIK.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex UNIWERSALNY_STEROWNIK.elf  "UNIWERSALNY_STEROWNIK.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: UNIWERSALNY_STEROWNIK.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega644p UNIWERSALNY_STEROWNIK.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) UNIWERSALNY_STEROWNIK.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
