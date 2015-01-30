################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
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
DS1621_obsluga.lss \

FLASH_IMAGE += \
DS1621_obsluga.hex \

EEPROM_IMAGE += \
DS1621_obsluga.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: DS1621_obsluga.elf secondary-outputs

# Tool invocations
DS1621_obsluga.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,DS1621_obsluga.map -mmcu=atmega128 -o"DS1621_obsluga.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

DS1621_obsluga.lss: DS1621_obsluga.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S DS1621_obsluga.elf  >"DS1621_obsluga.lss"
	@echo 'Finished building: $@'
	@echo ' '

DS1621_obsluga.hex: DS1621_obsluga.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex DS1621_obsluga.elf  "DS1621_obsluga.hex"
	@echo 'Finished building: $@'
	@echo ' '

DS1621_obsluga.eep: DS1621_obsluga.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex DS1621_obsluga.elf  "DS1621_obsluga.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: DS1621_obsluga.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128 DS1621_obsluga.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) DS1621_obsluga.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
