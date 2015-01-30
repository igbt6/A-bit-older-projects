/*
 * i2c_twi.h
 *
 *  Created on: 04-12-2012
 *      Author: £ukasz Uszko
*/


#ifndef I2C_TWI_H_
#define I2C_TWI_H_
#include <stdbool.h>
#define ACK 1
#define NACK 0

// raejestr stanow magistrali (TWI Status Register)
#define	 START				0x08
#define  REPEAT_START		0x10
#define  MT_SLA_ACK			0x18
#define  MT_SLA_NACK		0x20
#define  MT_DATA_ACK		0x28
#define  MT_DATA_NACK		0x30
#define  MR_SLA_ACK			0x40
#define  MR_SLA_NACK		0x48
#define  MR_DATA_ACK		0x50
#define  MR_DATA_NACK		0x58
#define  ARB_LOST			0x38
#define  ERROR_CODE			0x7e
// funkcje

void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8_t bajt);
uint8_t I2C_read(uint8_t ack);
void I2C_init(uint32_t speed);
bool I2C_write_data(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t data);
uint8_t I2C_read_data(uint8_t SLA, uint8_t adr, uint8_t len);

#endif /* I2C_TWI_H_ */
