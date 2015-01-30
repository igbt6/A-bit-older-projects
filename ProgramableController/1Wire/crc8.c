/************************************************************************/
/*                                                                      */
/*        Access Dallas 1-Wire Device with ATMEL AVRs                   */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/* modified by Martin Thomas <eversmith@heizung-thomas.de> 9/2004       */
/************************************************************************/
/*
 * crc8.c
 *
 */
/* please read copyright-notice at EOF */

#include <inttypes.h>

#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0

uint8_t	crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read )
{
	uint8_t	 crc;
	uint16_t loop_count;
	uint8_t  bit_counter;
	uint8_t  data;
	uint8_t  feedback_bit;

	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
	{
		data = data_in[loop_count];

		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;

			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}

			data = data >> 1;
			bit_counter--;

		} while (bit_counter > 0);
	}

	return crc;
}



