/*projekt sterownik oœwietlenia sterowany pilotem od dekodera sat SAMSUNG lub inne programowalne  - - atmega 8/// F_CPU=8MHz  ZEWNÊTRZNY REZONATOR
 * main.c
 *
 *  Created on: 2012-09-07
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "triak.h"
#include "common.h"
#include "DEKODER_IR.h"
#include <stdbool.h>
typedef uint8_t u08;
typedef uint16_t u16;

void init_ADC(void); //deklaracje funkcji
void init_TIMER2(void);
volatile u08 Timer1, Timer2, Timer3, Timer4, Timer5; // timery programowe 100Hz
volatile bool flaga_uspienia; // flagi przydatne do czasowego uspienia
volatile bool flaga_hour; //


volatile u16 value_ADC; // pomocnicza zmienna do obróbki wartoœci z ADCW
/*********************************************************************************************************/
int main(void) {
	DDRC |= (1 << PC0); // zarowka
	DDRC &= ~(KL_ON_OFF | KL_UP | KL_DOWN); // wejœcia
	PORTC |= (KL_ON_OFF | KL_UP | KL_DOWN); //podci¹gam do pullupów , bede sprwdza³ stan niski
	DDRB |= (LED_GREEN | LED_RED );
	DDRD &= ~(1 << PD5);
	PORTD |= 1 << PD5; // switch narazie do testow
	init_ADC(); //inicjalizacja przetwornika ADC
	init_interruptINT0();
	IR_DIR &= ~IR_PIN; // pin IR jako wejœcie, poniewa¿ jest domyœlnie to pomijamy
	IR_PORT |= IR_PIN; // podci¹gniêcie wejœcia IR do VCC
	Init_Timer1();// KONFIGURACJA PRACY PRZERWANIA ICP I TIMERA1
	init_TIMER0();
	init_TIMER2();
//	USART_Init(__UBRR); // inicjalizacja UART DEBUGERRRRRRRRRRRRRR
	T_ZAR_OFF;
	PORTB|=LED_GREEN;
	enum stan_klawisza {
		nie_wcisniety, wcisniety, trzymany, oczekujacy
	};
	static enum stan_klawisza stan_KL_ON_OFF, stan_KL_UP, stan_KL_DOWN,
			stan_KL_IR_ON_OFF;//, stan_KL_IR_DOWN, stan_KL_IR_UP;
	static u08 flaga_on_off = 0; // mowi czy wylaczone czy nie
	static u08 liczba_procent = 100;
	stan_KL_IR_ON_OFF = trzymany;
	stan_KL_ON_OFF = trzymany; // nadaje poczatkowe wartosci

	sei();
	// enable global interrupts

	ACSR |= (1 << ACD); // wy³¹czenie komparatora analogowego konieczne ze wzglêdu na maksymalne obni¿enie poboru pr¹du
	while (1) {

		switch (flaga_on_off) {

		case 0: //(flaga_on_off==0)//gdy wy³¹czony zgaœ diody , uœpij
		{

			if (stan_KL_IR_ON_OFF == trzymany || stan_KL_ON_OFF == trzymany) { // po powrocie sie wykonuje tylko raz
				//	uart_put_int((uint16_t) stan_KL_IR_ON_OFF);//DEBUGERRRRRRRRR
				//	uart_puts(" ");//DEBUGERRRRRRRRR
				//	uart_put_int((uint16_t) stan_KL_ON_OFF); //DEBUGERRRRRRRRR
				Timer1 = 100;
				while (Timer1 != 0) {
				};
				flaga_uspienia = true;// licznik bezczynnosci zeruje
				PORTB &= ~(LED_GREEN | LED_RED);
				liczba_procent = 100;
				brightness_bulb = procent(0);// gaze zarowke
				stan_KL_IR_ON_OFF = nie_wcisniety;
				stan_KL_ON_OFF = oczekujacy;

			}
			///********************************************************* tutaj obsluga klawiszy (ON/OFF)*****************************************************************

			if (!(PINC & KL_ON_OFF) && (stan_KL_ON_OFF == oczekujacy)) {
				if (!(PINC & KL_ON_OFF)) {
					Timer1 = 10; // czekam 0.1sek jak jest to to sprawdzam jeszcze czy bedzie 2.5 jak nie to wlaczam urzadzenie
					while (Timer1 != 0) {
					};
					if (!(PINC & KL_ON_OFF)) {
						Timer1 = 250; // czekam 2.5sek jak bedzie spelnione to uruchamiam programowanie
						while (Timer1 != 0) {
						};
						if (!(PINC & KL_ON_OFF)) {
							Programing_NewPilot_IR();
						} else {
							if ((value_ADC <= 900)) {
								stan_KL_ON_OFF = wcisniety;
							} else if (value_ADC > 900) {
								PORTB |= LED_RED;
								Timer1 = 70;
								while (Timer1 != 0) {
								};
								PORTB &= ~(LED_RED); //jak jest za jasno nie da rady w³¹czyæ

							}

						} // czyli urzadzenie do wlaczenia
					} else
						stan_KL_ON_OFF = nie_wcisniety;
				}
			}
			///********************************************************* tutaj jeœli odebra³em ramkê *****************************************************************
			if (frame_end) {
			//	INT0_STOP; // zablokowanie INT0
			//	TIMER0_STOP; // zablokowanie Timera0
				PORTB |= LED_RED; // ze odebralem ramke
				IR_DIR |= IR_PIN;
				eeprom_read_block(tab_in_eeprom, Tab_Code_EEPROM_IR_1,
						sizeof(Tab_Code_EEPROM_IR_1));
				if (Compare_2_Tab(tab_in_eeprom, tab, ILOSC_KODOW)) {
					//.................................................
					if ((stan_KL_IR_ON_OFF == nie_wcisniety) && (value_ADC
							<= 900)) {
						stan_KL_IR_ON_OFF = oczekujacy;
					} else if ((stan_KL_IR_ON_OFF == nie_wcisniety)
							&& (value_ADC //jak jest za jasno nie da rady wlaczyc
									> 900)) {

						PORTB |= LED_RED;
						Timer1 = 70;
						while (Timer1 != 0) {
						};
						PORTB &= ~(LED_RED);
					}
				}
				if (stan_KL_IR_ON_OFF == oczekujacy) {
					Timer2 = 100;
					while (Timer2 != 0) {
					};
					stan_KL_IR_ON_OFF = wcisniety;
				}
				PORTB &= ~(LED_RED); //
				Clear_1_Tab(tab, ILOSC_KODOW); // czyszcze tabICP z przerwania
				TIFR |= (1 << ICF1);// skasowanie flagi przerwania
				IR_DIR &= ~IR_PIN;
				TCCR1B &= ~(1 << ICES1); // reakcja na zbocze opadaj¹ce
				TIMSK |= (1 << TICIE1);// ponowne odblokowanie przerwania ICP1
				frame_end = 0;
				//INT0_START; // odblokowanie INT0
			}
			if (stan_KL_ON_OFF == wcisniety || stan_KL_IR_ON_OFF == wcisniety) {
				//uart_puts("switchon/off"); //DEBUGGERRRRRRRRRR
				//uart_puts(" case1 ");//DEBUGERRRRRRRRR
				PORTB |= (LED_GREEN) | (LED_RED);
				flaga_on_off = 1; // uruchamiam urzadzenie
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			break;
		}

			///if(flaga_on_off==1)// CZYLI STEROWNIK W£¥CZONY *********************************
		case 1: {
			///uart_puts(" case1 ");//DEBUGERRRRRRRRR
			flaga_uspienia = false;
			if (stan_KL_ON_OFF == wcisniety || stan_KL_IR_ON_OFF == wcisniety) { //opó¿nienie
				Timer1 = 70;
				while (Timer1 != 0) {
				};
				stan_KL_ON_OFF = oczekujacy;
				stan_KL_IR_ON_OFF = nie_wcisniety;
				PORTB &= ~(LED_RED);// zostaw tylko zielon¹ diodeê
			} //dopóki Timer1 bêdzie równy 0 bêdê czeka³


			brightness_bulb = procent(liczba_procent); // jasnosc zarowki
			//	uart_puts(" %: "); //DEBUGERRRRRRRRR
			//	uart_put_int((uint16_t) liczba_procent);//DEBUGERRRRRRRRR
			//	uart_puts(" sekundy: "); //DEBUGERRRRRRRRR
			//	uart_put_int(sekundy);//DEBUGERRRRRRRRR


			if (!(PINC & KL_ON_OFF) && (stan_KL_ON_OFF == oczekujacy)) {
				if (!(PINC & KL_ON_OFF)) {
					Timer3 = 10;
					while (Timer3 != 0) { //zw³oka i drgania styków
					};
					if (!(PINC & KL_ON_OFF)) {
						stan_KL_ON_OFF = trzymany;
					}
				}
			} else
				stan_KL_ON_OFF = oczekujacy;

			///////////////////////////////tu bedemy ryc
			if (frame_end) {
				//INT0_STOP; // zablokowanie INT0
				//TIMER0_STOP; // zablokowanie Timera0

				flaga_uspienia = true; /// czyli wykasuj licznik  jeszcze do rozkminy bo co bedzie gdy bedzie zaklocenia odbieral
				PORTB ^= LED_RED; // sygnalizacja ze odebral
				IR_DIR |= IR_PIN;
				while (1) {
					eeprom_read_block(tab_in_eeprom, Tab_Code_EEPROM_IR_1,
							sizeof(Tab_Code_EEPROM_IR_1));
					if (Compare_2_Tab(tab_in_eeprom, tab, ILOSC_KODOW)) {
						//	uart_puts(" PIERWSZA_IR_ON/OFF "); //DEBUGERRRRRRRRR
						if ((stan_KL_IR_ON_OFF == nie_wcisniety
								|| stan_KL_IR_ON_OFF == wcisniety)) {
							Timer3 = 15;
							while (Timer3 != 0) { //zw³oka 150ms  // to mo¿na kiedys dodac , zobaczymy jak sie sprawdzi wszystko
							};
							PORTB ^= LED_RED; // sygnalizacja ze odebral poprawna
							stan_KL_IR_ON_OFF = trzymany;
							break;
						}
					} else {
						eeprom_read_block(tab_in_eeprom, Tab_Code_EEPROM_IR_2,
								sizeof(Tab_Code_EEPROM_IR_2));
					}
					///====================================================IR_UP=======================================
					if (Compare_2_Tab(tab_in_eeprom, tab, ILOSC_KODOW)) {
						//		uart_puts(" DRUGA_IR_UP "); //DEBUGERRRRRRRRR

						Timer3 = 15;
						while (Timer3 != 0) { //zw³oka 150ms  // to mo¿na kiedys dodac , zobaczymy jak sie sprawdzi wszystko
						};
						PORTB ^= LED_RED; // sygnalizacja ze odebral poprawna
						if (/*!(stan_KL_IR_UP) && */(liczba_procent <= 95)) {
							///to co na niebiesko w kodzie to dla przyspieszenia bo po co to stosowac tutaj niby i tak samo na dolee dla IR_UP
							/*!	stan_KL_IR_UP = wcisniety;
							 }
							 if (stan_KL_IR_UP == wcisniety) {*/
							liczba_procent += 4;
							///	stan_KL_IR_UP = nie_wcisniety;
							if (liczba_procent >= 95) {
								liczba_procent = 100;
								break;
							}

						}
						break;
					} else {
						eeprom_read_block(tab_in_eeprom, Tab_Code_EEPROM_IR_3,
								sizeof(Tab_Code_EEPROM_IR_3));
					}
					///=========================================IR_DOWN= czyli TAB3====================================================
					if (Compare_2_Tab(tab_in_eeprom, tab, ILOSC_KODOW)) {

						Timer3 = 15;
						while (Timer3 != 0) { //zw³oka 150ms  // to mo¿na kiedys dodac , zobaczymy jak sie sprawdzi wszystko
						};
						PORTB ^= LED_RED; // sygnalizacja ze odebral poprawna
						//	uart_puts(" TRZECIA_IR_DOWN ");
						if ((liczba_procent >= 5) /**&& !(stan_KL_IR_DOWN)*/) { //nie chce mi siê tu tych stanów sprawdzaæ , bo i po co
							/*!	stan_KL_IR_DOWN = wcisniety;
							 }
							 if (stan_KL_IR_DOWN == wcisniety) {*/
							liczba_procent -= 4;
							///	stan_KL_IR_DOWN = nie_wcisniety;
							if (liczba_procent < 5) {
								liczba_procent = 0;
							}
						}
						break;
					} else {
						PORTB ^= LED_RED; // sygnalizacja ze odebral niepoprawna
						break;
					}
				}
				//		Timer3 = 15;
				//		while (Timer3 != 0) { //zw³oka 150ms  // to mo¿na kiedys dodac , zobaczymy jak sie sprawdzi wszystko
				//		};
				Clear_1_Tab(tab, ILOSC_KODOW); // czyszcze tabICP z przerwania
				TIFR |= (1 << ICF1);// skasowanie flagi przerwania
				IR_DIR &= ~IR_PIN;
				TCCR1B &= ~(1 << ICES1); // reakcja na zbocze opadaj¹ce
				frame_end = 0;
				TIMSK |= (1 << TICIE1); // ponowne odblokowanie przerwania ICP1
				//INT0_START; // odblokowanie INT0
			}

			if (stan_KL_IR_ON_OFF == trzymany || stan_KL_ON_OFF == trzymany) {
				flaga_on_off = 0;
				//	uart_puts(" case0 ");//DEBUGERRRRRRRRR

			}
			//------------------------------------------------------------------------------------------------/
			//-----------------------------------------------------licznik bezczynnoœci--------------------------------/


			if (flaga_hour == true) {
				stan_KL_IR_ON_OFF = trzymany;
				stan_KL_ON_OFF = trzymany;
				flaga_on_off = 0;
				flaga_hour = false;
			}// po dwóch godzinach bez wciskania klawiszy siê wy³¹czy
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////KL_DOWN & KL_UP///////////////////////////////////////////////////////


			///////////////////////////////////////////////KL_DOWN//////////////////////////////////////////////////////////

			if (!(PINC & KL_DOWN) && !(stan_KL_DOWN) && (liczba_procent > 0)) {
				if (!(PINC & KL_DOWN)) {
					Timer4 = 7;
					while (Timer4 != 0) { //odczekam sobie ma³¹ przerwê
					};
					if (!(PINC & KL_DOWN)) {

						liczba_procent--;
						if (liczba_procent <= 5) {
							liczba_procent = 0;

						}
					}
				}
			} else
				stan_KL_DOWN = nie_wcisniety;

			/////////////////////////////////////////////////////////KL_UP//////////////////////////////////////////////////
			if (!(PINC & KL_UP) && !(stan_KL_UP) && liczba_procent < 100) {
				if (!(PINC & KL_UP)) {
					Timer5 = 7;
					while (Timer5 != 0) {
					};
					if (!(PINC & KL_UP)) {
						//stan_KL_DOWN = wcisniety;
						liczba_procent++;
						if (liczba_procent >= 97) {
							liczba_procent = 100;
							//	break;
						}
					}
				}
			} else
				stan_KL_UP = nie_wcisniety;

			break;
		}
		}

	}

}

//***********************************************************************************************************
void init_ADC(void) { //wybranie kana³u ADC w tym przypadku 0101 - kana³ pi¹ty
	ADMUX |= (1 << REFS0 | 1 << REFS1 | 1 << MUX2 | 1 << MUX0); //wybór napiêcia odnesienia w postaci 2,56V
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //w³¹czenie modu³u ADC oraz ustawienie preskalera 128-->8MHZ/128 = 62,5kHZ
	ADCSRA |= 1 << ADFR | 1 << ADIE; // tryb free running mode i w³¹czenie przerwania
	ADCSRA |= 1 << ADSC; // start pomiaru
}
//************************************************************************************************************
void init_TIMER2(void) {
	TCCR2 |= (1 << CS22 | 1 << CS21 | 1 << CS20); //prescaler 1024 // Timer 100Hz//F_CPU=8MHz
	TCCR2 |= (1 << WGM21); // tryb CTC
	TIMSK |= (1 << OCIE2); // odblokowanie przerwania COMPARE MATCH
	OCR2 = 78;
}

//********************************************************************************************************************
ISR(ADC_vect)
{
	value_ADC = ADCW;
}
//*************************************************************************************************************
ISR(TIMER2_COMP_vect)
{
	u08 n;
	static u16 liczba_sek; //1 min gdy TCNT1=6000;
	static u08 liczba_min;

	n = Timer1; /* 100Hz Timer1 */
	if (n)
		Timer1 = --n;

	n = Timer2;
	if (n > 0) {
		--n;
		Timer2 = n;
	}

	n = Timer3; /* 100Hz Timer3 */
	if (n > 0) {
		--n;
		Timer3 = n;
	}
	n = Timer4; /* 100Hz Timer4 */
	if (n > 0) {
		--n;
		Timer4 = n;
	}
	n = Timer5; /* 100Hz Timer4 */
	if (n > 0) {
		--n;
		Timer5 = n;
	}/* 100Hz Timer5 */
	if (flaga_uspienia == false) {
		liczba_sek++;
		if (liczba_sek == 6000) {
			liczba_min++;
			liczba_sek = 0;
		}
		if (liczba_min == 120) { //minely 2 godziny
			flaga_hour = true;
			liczba_min = 0;
			liczba_sek = 0;
		}
	}
	if (flaga_uspienia == true) {
		liczba_min = 0;
		liczba_sek = 0;
		flaga_hour = false;
		flaga_uspienia = false;
	}
}

