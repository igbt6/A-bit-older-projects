/*
 * adc.c
 *
 *  Created on: 2012-03-04
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hd44780.h"


uint16_t pomiar(uint8_t kanal);    //deklaracja funkcji pomiaru
volatile uint16_t value;

	uint32_t wynik;
	uint16_t wart_zmierzona;

	uint8_t cz_d, cz_u;


   /* ADMUX|=  5;                                           //wybranie kana³u ADC w tym przypadku 0101 - kana³ pi¹ty
	ADMUX|=(1<<REFS0|1<<REFS1|5);                         //wybór napiêcia odnesienia w postaci 2,56V
	ADCSRA|= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);             //w³¹czenie modu³u ADC oraz ustawienie preskalera 64-->8MHZ/64
	ADCSRA|=1<<ADFR|1<<ADIE;                              // tryb free running mode i w³¹czenie przerwania
	ADCSRA|=1<<ADSC;                                      // start pomiaru
*/


/*uint16_t pomiar(uint8_t kanal){

	ADMUX|= (ADMUX | kanal);          //ustawienie wybranego kana³u ADC
	ADCSRA|=1<<ADSC;                  //start pomiaru
	while(ADCSRA&(1<<ADSC));          //oczekiwanie na koniec pomiaru dopóki ADSC  jest =1
	return ADCW;                      // zwrócenie 16 bitowej wartoœci z rejestrów ADCH i ADCL
}*/

 ISR(ADC_vect)                        // obs³uga przerwania od ADC
{
	 value =ADCW;
}
