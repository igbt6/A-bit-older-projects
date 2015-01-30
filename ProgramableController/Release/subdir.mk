################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USART.c \
../adc.c \
../alarms.c \
../ds18b20_obsluga.c \
../harddef.c \
../hd44780.c \
../i2c_twi.c \
../main.c \
../menu.c \
../pcf8583.c \
../triak.c 

OBJS += \
./USART.o \
./adc.o \
./alarms.o \
./ds18b20_obsluga.o \
./harddef.o \
./hd44780.o \
./i2c_twi.o \
./main.o \
./menu.o \
./pcf8583.o \
./triak.o 

C_DEPS += \
./USART.d \
./adc.d \
./alarms.d \
./ds18b20_obsluga.d \
./harddef.d \
./hd44780.d \
./i2c_twi.d \
./main.d \
./menu.d \
./pcf8583.d \
./triak.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


