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
 * ds18x20.c
 *
 *  Created on: 2009-08-22
 *      modyfikacje: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>

#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"




uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];



uint8_t DS18X20_meas_to_cel( uint8_t fc, uint8_t *sp,
	uint8_t* subzero, uint8_t* cel, uint8_t* cel_frac_bits)
{
	uint16_t meas;
	uint8_t  i;
	uint8_t t_tab1[16] = {0,1,1,2,2,3,4,4,5,6,6,7,7,8,9,9};

	meas = sp[0];  // LSB
	meas |= ((uint16_t)sp[1])<<8; // MSB
	//meas = 0xff5e; meas = 0xfe6f;

	//  only work on 12bit-base
	if( fc == DS18S20_ID ) { // 9 -> 12 bit if 18S20
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
		meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
		meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
	}

	// check for negative
	if ( meas & 0x8000 )  {
		*subzero=1;      // mark negative
		meas ^= 0xffff;  // convert to positive => (twos complement)++
		meas++;
	}
	else *subzero=0;

	// clear undefined bits for B != 12bit
	if ( fc == DS18B20_ID ) { // check resolution 18B20
		i = sp[DS18B20_CONF_REG];
		if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;
		else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT )
			meas &= ~(DS18B20_11_BIT_UNDF);
		else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT )
			meas &= ~(DS18B20_10_BIT_UNDF);
		else { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
			meas &= ~(DS18B20_9_BIT_UNDF);
		}
	}

	*cel  = (uint8_t)(meas >> 4);
	*cel_frac_bits = t_tab1[(uint8_t)(meas & 0x000F)]    ;


	return DS18X20_OK;
}



/* compare temperature values (full celsius only)
   returns -1 if param-pair1 < param-pair2
            0 if ==
			1 if >    */
int DS18X20_temp_cmp(uint8_t subzero1, uint16_t cel1,
	uint8_t subzero2, uint16_t cel2)
{
	int16_t t1 = (subzero1) ? (cel1*(-1)) : (cel1);
	int16_t t2 = (subzero2) ? (cel2*(-1)) : (cel2);

	if (t1<t2) return -1;
	if (t1>t2) return 1;
	return 0;
}

/* find DS18X20 Sensors on 1-Wire-Bus
   input/ouput: diff is the result of the last rom-search
   output: id is the rom-code of the sensor found */
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
	for (;;) {
		*diff = ow_rom_search( *diff, &id[0] );
		if ( *diff==OW_PRESENCE_ERR || *diff==OW_DATA_ERR ||
		  *diff == OW_LAST_DEVICE ) return;
		if ( id[0] == DS18B20_ID || id[0] == DS18S20_ID ) return;
	}
}



uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;

	nSensors = 0;

	for( diff = OW_SEARCH_FIRST;
		diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );

		if( diff == OW_PRESENCE_ERR ) {
			break;
		}

		if( diff == OW_DATA_ERR ) {
			break;
		}

		for (i=0;i<OW_ROMCODE_SIZE;i++) gSensorIDs[nSensors][i]=id[i];

		nSensors++;
	}

	return nSensors;
}




/* get power status of DS18x20
   input  : id = rom_code
   returns: DS18X20_POWER_EXTERN or DS18X20_POWER_PARASITE */
uint8_t	DS18X20_get_power_status(uint8_t id[])
{
	uint8_t pstat;
    ow_reset();
    ow_command(DS18X20_READ_POWER_SUPPLY, id);
    pstat=ow_bit_io(1); // pstat 0=is parasite/ !=0 ext. powered
    ow_reset();
	return (pstat) ? DS18X20_POWER_EXTERN:DS18X20_POWER_PARASITE;
}

/* start measurement (CONVERT_T) for all sensors if input id==NULL
   or for single sensor. then id is the rom-code */
uint8_t DS18X20_start_meas( uint8_t with_power_extern, uint8_t id[])
{
	ow_reset(); //**
	if( ow_input_pin_state() ) { // only send if bus is "idle" = high
		ow_command( DS18X20_CONVERT_T, id );
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		return DS18X20_OK;
	}
	else {

		return DS18X20_START_FAIL;
	}
}

/* reads temperature (scratchpad) of sensor with rom-code id
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas(uint8_t *id, uint8_t *subzero, uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];

	ow_reset(); //**
	ow_command(DS18X20_READ, id);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}

/* reads temperature (scratchpad) of a single sensor (uses skip-rom)
   output: subzero==1 if temp.<0, cel: full celsius, mcel: frac
   in millicelsius*0.1
   i.e.: subzero=1, cel=18, millicel=5000 = -18,5000°C */
uint8_t DS18X20_read_meas_single(uint8_t familycode, uint8_t *subzero, uint8_t *cel, uint8_t *cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];

	ow_command(DS18X20_READ, NULL);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(familycode, sp, subzero, cel, cel_frac_bits);


	return DS18X20_OK;
}


