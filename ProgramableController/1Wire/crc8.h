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
 * crc8.h
 *
 */

#ifndef CRC8_H_
#define CRC8_H_

#include <inttypes.h>

uint8_t	crc8 (uint8_t* data_in, uint16_t number_of_bytes_to_read);

#endif


