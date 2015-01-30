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
 * onewire.h
 *
 *  Created on: 2009-08-22
 *      modyfikacje: Miros³aw Kardaœ i £ukasz Uszko
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <inttypes.h>

/*******************************************/
/* Hardware connection                     */
/*******************************************/

/* Wybór PINu oraz PORTu na magistralê 1Wire */
#define OW_PIN  PC2
#define OW_IN   PINC
#define OW_OUT  PORTC
#define OW_DDR  DDRC



/*******************************************/

#define OW_MATCH_ROM	0x55
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_ROM	0xF0

#define	OW_SEARCH_FIRST	0xFF		// start new search
#define	OW_PRESENCE_ERR	0xFF
#define	OW_DATA_ERR	    0xFE
#define OW_LAST_DEVICE	0x00		// last device found
//			0x01 ... 0x40: continue searching

// rom-code size including CRC
#define OW_ROMCODE_SIZE 8

uint8_t ow_reset(void);

uint8_t ow_bit_io( uint8_t b );
uint8_t ow_byte_wr( uint8_t b );
uint8_t ow_byte_rd( void );

uint8_t ow_rom_search( uint8_t diff, uint8_t *id );

void ow_command( uint8_t command, uint8_t *id );

void ow_parasite_enable(void);
void ow_parasite_disable(void);
uint8_t ow_input_pin_state(void);



#endif /* ONEWIRE_H_ */
