/*
 * harddef.c
 *Plik w którym znajduj¹ siê funkcje obs³ugi przerwañ , GPIO, itp itd
 *  Created on: 2012-02-11
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "harddef.h"

volatile uint16_t Timer1, Timer2, Timer3; // timery programowe 1000Hz
volatile uint8_t sekundnik; //licznik sekund
volatile bool int0_flag = 1; // flaga zmieniana w przerwaniu i sprawdzana w pêtli g³ównej
void GPIO_Init(void) {
	DDRC
			&= ~(SWITCH_OK | SWITCH_GORA | SWITCH_DOL | SWITCH_LEWO
					|SWITCH_PRAWO); // switche wejœcia
	PORTC
			|= (SWITCH_OK | SWITCH_GORA | SWITCH_DOL | SWITCH_LEWO
					|SWITCH_PRAWO); // z podci¹gniêciem do plusa
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//funkcja czytaj¹ca stan wejœcia
//PARAMS: pin-->czyli nr pinu np.: SWITCH_OK=1<<7
//RESULT: 0 jeœli na wejœciu stan wysoki czyli switch nie wciœniêty
//     1 jeœli na wejœciu stan niski  ---> switch wcisniêty

bool Read_Input_Switch(uint8_t pin) {
	//uint8_t counter = 0;
	//while (1) {

	if (!(PINC & pin)) {
		Delay_ms(30);
		//	counter = 1;
		if (!(PINC & pin)) {
			return true;
		} else
			return false;
	} else
		//if ((PINC & pin)/* && counter == 0*/)
		return false;

	//}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************
 * funkcja sprawdzaj¹ca stan wejœæ
 * IN: PIN(x) z jakich czytam
 * RETURN: PIN który zosta³ wciœniêty
 ******************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Read_Pin_Input_Switch(void) {
	uint8_t przycisk=0;
	if (!(PINC & SWITCH_DOL)) {
		Delay_ms(40);
		if (!(PINC & SWITCH_DOL)) {
			przycisk = SWITCH_DOL;
		}
	} else if (!(PINC & SWITCH_GORA)) {
		Delay_ms(40);
		if (!(PINC & SWITCH_GORA)) {
			przycisk = SWITCH_GORA;
		}
	} else if (!(PINC & SWITCH_OK)) {
		Delay_ms(40);
		if (!(PINC & SWITCH_OK)) {
			przycisk = SWITCH_OK;
		}
	} else if (!(PINC & SWITCH_LEWO)) {
		Delay_ms(40);
		if (!(PINC & SWITCH_LEWO)) {
			przycisk = SWITCH_LEWO;
		}
	} else if (!(PINC & SWITCH_PRAWO)) {
		Delay_ms(40);
		if (!(PINC & SWITCH_PRAWO)) {
			przycisk = SWITCH_PRAWO;
		}
	} else
		przycisk = 0;
	return przycisk;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void INT0_init(void) { //init INT0 od RTC co 1 sek
	EICRA = (1 << ISC00 | 1 << ISC01); // rising edge
	EIMSK = (1 << INT0); //ENABLE
	DDRD &= ~(1 << DDD2);
	PORTD |= (1 << PORT2); // podci¹gam do plusa bo wyjœcie INT w pcfie jest open drain
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void TIMER0_init(void) {
	TCCR0A |= (0 << COM0A0 | 0 << COM0A1 | 0 << COM0B0 | 0 << COM0B1 | 1
			<< WGM01); //Normal port operation, OC0A & OCOB disconnected.//tryb CTC
	//TCCR0B |= (1 << CS02); //prescaler/256                                  //DLA FCPU 8MHz
	//OCR0A = 31;//8MHz/256=31250--->przerwanie co 1ms czyli 1000Hz          //DLA FCPU 8MHz
	//OCR0B = 156;//przerwanie co 200z czyli 5ms

	TCCR0B|=(1<<CS01|1<<CS00);                                              //DLA FCPU 16MHz prescaler 64
	OCR0A = 250;//16MHz/64=250000--->przerwanie co 1ms czyli 1000Hz          //DLA FCPU 8MHz
	TIMSK0 |= (1 << OCIE0A); // odblokowanie przerwania COMPARE MATCH
}
/////////////////////////////////////////////////////////////////////////////////////////////////// do dupy funkcje ZMIENIÆ CZA
bool Delay_ms(uint16_t czas_1_ms) {
	Timer1 = czas_1_ms; // opóŸnienie o zadan¹ liczbe ms
	while (Timer1 != 0)
		;
	return true ;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool Delay_sec(uint16_t czas_1_sec) {
	Timer2 = czas_1_sec * 1000; // opóŸnienie o zadan¹ liczbe sekund
	while (Timer2 != 0)
		;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool Delay_100ms(uint16_t czas_100_ms) {
	Timer3 = czas_100_ms * 100; // opóŸnienie o zadan¹ liczbe 100ms
	while (Timer3 != 0)
		;
	return true ; // zawraca prawdê jak minie zadany czas
}
//////////////////////////////////////////////////////////////////////////////////////////////////
/*void Delay_Debounce(void) {
 for (uint32_t i = 300000; i > 0; i--)
 ; //300000 bo dla 8Mhz jest to oko³o 37 ms
 }*/

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////FUNKCJE OBS£UGI PRZERWAÑ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT0_vect)
{
	int0_flag = 1;
	++sekundnik;
	if (sekundnik > 60) { //przerwanie co równ¹ sekundê wykorzystam w odzczycie
		sekundnik = 0;
	} //temp z czujnika Ds18b20


}
//////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER0_COMPA_vect)
{
	TCNT0 = 0; // zeruje licznik od razu ¿eby nie skaka³
	//static uint8_t counter_switch;
	uint16_t n;
	n = Timer1; /* 1000Hz Timer1 */
	if (n)
		Timer1 = --n;
	//else TCCR0B=0;


	n = Timer2;
	if (n > 0) {
		--n;
		Timer2 = n;
	}

	n = Timer3;
	if (n > 0) {
		--n;
		Timer3 = n;
	}

}
