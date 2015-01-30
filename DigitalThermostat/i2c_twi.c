/*
 * i2c_twi.c
 *
 *  Created on: 04-12-2012
 *      Author: £ukasz Uszko
 */

#include <avr/io.h>
#include "i2c_twi.h"

void I2C_init(uint32_t speed) { // speed - predkosc magistrali I2C w Hz
	uint32_t x;
	uint32_t f_cpu = F_CPU;
	x = ((f_cpu / speed) / 2) - 16;
	TWBR |= x;
	TWSR |= (0 << TWPS1 | 0 << TWPS0); // prescaler =0
}

void I2C_start(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	while (!(TWCR & (1 << TWINT)))
		;
}

void I2C_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	while (!(TWCR & (1 << TWSTO)))
		;
}

void I2C_write(uint8_t bajt) {
	TWDR = bajt;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
}

uint8_t I2C_read(uint8_t ack) {
	TWCR = (1 << TWINT) | (ack << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return TWDR;
}
/************************************************************************************************
 funkcja wysy³aj¹ca dane do uk³adu SLAVE
 IN:*SLA-adres uk³ slave
 *adr-adres pamiêci RAM slave'a od którego chce rozpocz¹æ zapisywanie
 *len-iloœæ przesy³anych bajtów
 *data - dane(bajty)
 OUT:NONE
 ************************************************************************************************/bool I2C_write_data(
		uint8_t SLA, uint8_t adr, uint8_t len, uint8_t data) {

	I2C_start();
	I2C_write(SLA);
	I2C_write(adr);
	//while (len--)
	I2C_write(data);
	if ((TWSR & 0xF8) != MT_DATA_ACK) { //Check value of TWI Status Register
		I2C_stop();
		return (true);
	} else
		I2C_stop();
	return (false);

}

uint8_t I2C_read_data(uint8_t SLA, uint8_t adr, uint8_t len) {
	uint8_t data;
	I2C_start();
	I2C_write(SLA);
	I2C_write(adr);
	I2C_start();
	I2C_write(SLA ^= 1); //prze³¹cz adres  na ten do odczytu // czyli tylko najmlodszy bit
//	while (len--) {   // tutaj i tak tylko 1 bajt slac bede7
	data = I2C_read(/*len ? ACK :*/NACK); //jeœli jeszcze jakieœ bajty zczytuje, wysy³aj ka¿dy zakoñczony potwierdzenie ACK jeœli koniec to yœli NACK
//	}
	if ((TWSR & 0xF8) != MR_DATA_NACK) { //Check value of TWI Status Register
		I2C_stop();
		return (ERROR_CODE);
	} else
		return data;
}
