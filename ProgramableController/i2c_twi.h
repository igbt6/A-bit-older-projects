/*
 * i2c_twi.h
 *
 *  Created on: 2010-09-07
 *       Autor: Miros³aw Kardaœ
 */

#ifndef I2C_TWI_H_
#define I2C_TWI_H_

#define ACK 1
#define NACK 0
#define PCF8583_ADDR 0xA0



// funkcje

void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8_t bajt);
uint8_t I2C_read(uint8_t ack);
void I2C_init(uint32_t speed);
void I2C_write_data(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *data);
void I2C_read_data(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *data);

#endif /* I2C_TWI_H_ */
