/*
 * i2c_twi.c
 * funkcje do obs�ugi uk�adu magistrali TWI(I2C)
 *
 *  Created on: 2012-02-07
 *       Autor: Miros�aw Karda�  // poprawki i uzupe�nienia �ukasz Uszko
 */
#include <avr/io.h>

#include "i2c_twi.h"

void I2C_init(uint32_t speed) { // speed - predkosc magistrali w Hz
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
 funkcja wysy�aj�ca dane do uk�adu SLAVE
 IN:*SLA-adres uk� slave
 *adr-adres pami�ci RAM slave'a od kt�rego chce rozpocz�� zapisywanie
 *len-ilo�� przesy�anych bajt�w
 *data - dane(bajty)
 OUT:NONE
 ************************************************************************************************/
void I2C_write_data(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *data) {

	I2C_start();
	I2C_write(SLA);
	I2C_write(adr);
	while (len--)
		I2C_write(*data++);
	I2C_stop();
}

void I2C_read_data(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *data) {

	I2C_start();
	I2C_write(SLA);
	I2C_write(adr);
	I2C_start();
	I2C_write(SLA + 1); //prze��cz adres  na ten do odczytu
	while (len--) {
		*data++ = I2C_read(len ? ACK : NACK); //je�li jeszcze jakie� bajty zapisuje wysy�aj ka�dy zako�czony potwierdzenie ACK je�li koniec to y�li NACK
	}
	I2C_stop();
}

