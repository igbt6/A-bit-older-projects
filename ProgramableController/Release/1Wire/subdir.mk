################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../1Wire/crc8.c \
../1Wire/ds18x20.c \
../1Wire/onewire.c 

OBJS += \
./1Wire/crc8.o \
./1Wire/ds18x20.o \
./1Wire/onewire.o 

C_DEPS += \
./1Wire/crc8.d \
./1Wire/ds18x20.d \
./1Wire/onewire.d 


# Each subdirectory must supply rules for building sources it contributes
1Wire/%.o: ../1Wire/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


