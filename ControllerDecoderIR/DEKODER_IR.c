/*
 * DEKODER_IR.c
 *
 *  Created on: 2012-04-18
 *       Autor: £ukasz Uszko
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "stdbool.h"
#include <stdlib.h>
#include "DEKODER_IR.h"
#include <avr/eeprom.h>
#include "common.h"

extern uint16_t EEMEM Tab_Code_EEPROM_IR_1[ILOSC_KODOW]; // adresy eeprom
extern uint16_t EEMEM Tab_Code_EEPROM_IR_2[ILOSC_KODOW];
extern uint16_t EEMEM Tab_Code_EEPROM_IR_3[ILOSC_KODOW];

//*****************************************************************************************
//********************PRZERWANIA TIMER1****************************************************
//*****************************************************************************************

ISR(TIMER1_CAPT_vect)
{
	TIFR |= (1 << OCF1A); // kasuje flage od OCR1A
	TIMSK |= (1 << OCIE1A); // uruchamiam timer1 CTC tryb do sprawdzenia przy zad³ugiej przerwie koñcze ramkê
	TCCR1B ^= (1 << ICES1); // zmieniam zbocze na kolejne
	if (step == 0) {  // dla pierwszego wystapienia zbocza zeruje jeszcze czasu nie zapisuje bo wiadomo ze jest on jakis z dupy
		//TCNT1 = 0;
		pulse_cnt = 0;
	}
	if (step > 0) { // tu juz dla kolejnych stepow zapisuje te czasy do tablicy bo to znaczy ze wystapia kolejne ramki
		tab[step - 1] = ICR1;
		pulse_cnt++;
	}
	if (step == (ILOSC_KODOW - 1)) { // jakby rama za du¿a by³a mendka
		step = 0;
		frame_end = 1; // potwierdzam zakonczenie odbioru ramki
		TIMSK &= ~(1 << OCIE1A); // wylaczam przerwanie od CTC timer1
		TCNT1 = 0;
		TIMSK &= ~(1 << TICIE1); // wylaczam przerwanie od ICP timer1
	}
	step++;
	TCNT1 = 0;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ISR( TIMER1_COMPA_vect )
{
	///PORTD ^= 1 << PD6; //DEBUGER
	step = 0;
	frame_end = 1; // potwierdzam zakonczenie odbioru ramki
	TIMSK &= ~(1 << OCIE1A); // wylaczam przerwanie od CTC timer1
	TCNT1 = 0;
	TIMSK &= ~(1 << TICIE1); // wylaczam przerwanie od ICP timer1

}

//*****************************************************************************************
//********************PRZERWANIA TIMER1 -  koniec******************************************
//*****************************************************************************************

void Programing_NewPilot_IR(void) {
	uint16_t tab1[ILOSC_KODOW];
	uint16_t tab2[ILOSC_KODOW];
	///uint16_t tab3[ILOSC_KODOW];//DLA WIEKSZYCH PROCESOROW
	uint16_t* tab_code_IR1 = tab1; // albo musze rzutowaæ wynik malloca na uint* albo zostawiam puste
	uint16_t* tab_code_IR2 = tab2;
	///uint16_t* tab_code_IR3 = tab3; //DLA WIEKSZYCH PROCESOROW
	Which_Key_is_Pressed = pierwszy;
	switch (Which_Key_is_Pressed) {
	case pierwszy: {

		while (1) { // pierwszy ON/OFF musi byc zaprogramowany
			Clear_3_Tab(tab_code_IR1, tab_code_IR2, tab, ILOSC_KODOW);
			/**	eeprom_write_block(tab_code_IR1, Tab_Code_EEPROM_IR_1,    //to se moge odblokowac, coby po nieudanej probie zaprogramowania wyzerowal obie
			 sizeof(Tab_Code_EEPROM_IR_1)); // tutaj zapis do eepromu  pustej tablicy wyzerowanej
			 eeprom_write_block(tab_code_IR1, Tab_Code_EEPROM_IR_2,
			 sizeof(Tab_Code_EEPROM_IR_2)); // tutaj zapis do eepromu  pustej tablicy
			 eeprom_write_block(tab_code_IR1, Tab_Code_EEPROM_IR_3,
			 sizeof(Tab_Code_EEPROM_IR_3)); // tutaj zapis do eepromu pustej tablicy
			 **/
		//	uart_puts(" 1A_BEFORE ");//DEBUGER
			Conf_and_Wait_for_Frame();
			tab_code_IR1 = Copy_Tab(tab_code_IR1, tab, ILOSC_KODOW);
		//	uart_puts(" 1B_BEFORE ");//DEBUGER
			Conf_and_Wait_for_Frame();
			tab_code_IR2 = Copy_Tab(tab_code_IR2, tab, ILOSC_KODOW);
			if (Compare_2_Tab(tab_code_IR1, tab_code_IR2, ILOSC_KODOW)) {
				eeprom_write_block(tab_code_IR1, Tab_Code_EEPROM_IR_1,
						sizeof(Tab_Code_EEPROM_IR_1)); // tutaj zapis do eepromu jeszcze sprawdze drug¹ tablicê
				Confirmation_Successful_Write_to_EEPROM();
				Which_Key_is_Pressed = drugi;
				break;
			} else
				Confirmation_Unsuccessful_Write_to_EEPROM();
			continue;
		}
	}
	case drugi: {
		Clear_3_Tab(tab_code_IR1, tab_code_IR2, tab, ILOSC_KODOW);
	//	uart_puts(" 2A_BEFORE ");//DEBUGER
		Conf_and_Wait_for_Frame();
		tab_code_IR2 = Copy_Tab(tab_code_IR2, tab, ILOSC_KODOW);
	//	uart_puts(" 2B_BEFORE ");//DEBUGER
		Conf_and_Wait_for_Frame();
		tab_code_IR1 = Copy_Tab(tab_code_IR1, tab, ILOSC_KODOW);
		if (Compare_2_Tab(tab_code_IR2, tab_code_IR1, ILOSC_KODOW)) {
			eeprom_write_block(tab_code_IR2, Tab_Code_EEPROM_IR_2,
					sizeof(Tab_Code_EEPROM_IR_2)); // tutaj zapis do eepromu jeszcze sprawdze drug¹ tablicê
			Confirmation_Successful_Write_to_EEPROM();
		} else

			Confirmation_Unsuccessful_Write_to_EEPROM();

		Which_Key_is_Pressed = trzeci;

	}
	case trzeci: {
		Clear_3_Tab(tab_code_IR1, tab_code_IR2, tab, ILOSC_KODOW);
	//	uart_puts(" 3A_BEFORE ");//DEBUGER
		Conf_and_Wait_for_Frame();
		tab_code_IR1 = Copy_Tab(tab_code_IR1, tab, ILOSC_KODOW);
	//	uart_puts(" 3B_BEFORE ");//DEBUGER
		Conf_and_Wait_for_Frame();
		tab_code_IR2 = Copy_Tab(tab_code_IR2, tab, ILOSC_KODOW);
		if (Compare_2_Tab(tab_code_IR1, tab_code_IR2, ILOSC_KODOW)) {
			eeprom_write_block(tab_code_IR1, Tab_Code_EEPROM_IR_3,
					sizeof(Tab_Code_EEPROM_IR_3)); // tutaj zapis do eepromu jeszcze sprawdze drug¹ tablicê
			Confirmation_Successful_Write_to_EEPROM();
		} else
			Confirmation_Unsuccessful_Write_to_EEPROM();
	}
	}
	TCCR1B &= ~(1 << ICES1); // reakcja na zbocze opadaj¹ce  // przed wyjsciem musze to zalozyc
	IR_DIR &= ~IR_PIN; //ustawiam wejœie ICP (PB0)
	TIMSK |= (1 << TICIE1); // odblokowanie przerwania ICP
	return;
}
/////////////////////////////////////////////////////////////////////////////
 void Conf_and_Wait_for_Frame(void) {
	for (uint8_t i = 6; i > 0; i--) {
		PORTB ^= LED_RED; /// tu oczywiœcie w zale¿noœci od diod w sterowniku
		_delay_ms(300);
	}
	TCCR1B &= ~(1 << ICES1); // reakcja na zbocze opadaj¹ce
	IR_DIR &= ~IR_PIN; //ustawiam wejœie ICP (PB0)
	TIMSK |= (1 << TICIE1); // odblokowanie przerwania ICP
	while (!frame_end)
		; // czekam az odbiore cala ramke
	frame_end = 0; // zeruje flage ¿e ramka odebrana
	IR_DIR |= IR_PIN;// wejscie ICP ustawiam na wejscie , bez tego nie do konca dziala z zalozeniami
	return;
}
///////////////////////////////////////////////////////////////////////////////////
 uint16_t* Copy_Tab(uint16_t* tab_to_fill,
		volatile uint16_t* tab_with_frame, uint8_t _size) {
	for (uint8_t i = 0; i < _size; i++) {
		tab_to_fill[i] = tab_with_frame[i]; // kopiuje wartosci do zaalokowanej tablicy
	}
	return tab_to_fill;
}
///////////////////////////////////////////////////////////////////////////////////
bool Compare_2_Tab(uint16_t* tab1, uint16_t* tab2, uint8_t _size) {
	bool flaga = 0;
	bool bit_toogle = 0; // raz sie moze roznic
	for (uint8_t i = 0; i < _size; i++) {
		if (i == _size - 1) {
			flaga = 1;
			break;
		}
		if (tab1[i] <= TOLERANCE && tab2[i] <= TOLERANCE) { // zabezpieczene przed ujemn¹ wartoœci¹ bo pracuje przecie na uintach , tam gdzie sie tablica bedzie koñczyæ
			continue;
		} else if ((((tab1[i] + TOLERANCE) < (tab2[i])) || ((tab1[i]
				- TOLERANCE) > (tab2[i]))) && bit_toogle == false) {
			bit_toogle = true; // moze sie jednym bitem roznic , pomijac bede bit toogle
			continue;
		} else if ((((tab1[i] + TOLERANCE) < (tab2[i])) || ((tab1[i]
				- TOLERANCE) > (tab2[i]))) && bit_toogle == true) {
			flaga = 0;
			break;
		} else {

			continue;
		}
	}
	return flaga;
}
//////////////////////////////////////////////////////////////////////////////
void Clear_1_Tab(volatile uint16_t* tab_ICP, uint8_t _size) {
	for (uint8_t i = 0; i < _size; ++i) {
	tab_ICP[i] = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////
void Clear_3_Tab(uint16_t* tab1, uint16_t* tab2,
		volatile uint16_t* tab_ICP, uint8_t _size) {
	for (uint8_t i = 0; i < _size; ++i) {
		tab1[i] = 0;
		tab2[i] = 0;
		tab_ICP[i] = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////
 void Confirmation_Successful_Write_to_EEPROM(void) { //static czyli widoczna tylko w tym pliku
	for (uint8_t i = 10; i > 0; i--) {
		PORTB ^= LED_RED; /// tu oczywiœcie w zale¿noœci od diod w sterowniku
		_delay_ms(800);
	}

	_delay_ms(1000);
}

//////////////////////////////////////////////////////////////////////////////
 void Confirmation_Unsuccessful_Write_to_EEPROM(void) {
	for (uint8_t i = 10; i > 0; i--) {
		PORTB ^= LED_RED; /// tu oczywiœcie w zale¿noœci od diod w sterowniku
		_delay_ms(100);
	}
	_delay_ms(1000);
}
//////////////////////////////////////////////////////////////////////////////
void Init_Timer1(void) {
	TCCR1A |= 1 << WGM12; // tryb ctc
	TCCR1B |= (1 << ICNC1); // redukcja szumów
	TCCR1B |= (1 << CS11); // ustawienie preskalera dla Timer1 = 8
	TCCR1B &= ~(1 << ICES1); // reakcja na zbocze opadaj¹ce
	TIMSK |= (1 << TICIE1); // odblokowanie przerwania ICP // coby zaraz po starcie odbierac dane
	///	TIMSK |= (1 << OCIE1A); // odblokowanie przerwania CompareA
	OCR1A = ir_micro_s(9000); // tutaj deklaruje czas po którym zostanie zakoñczona ramka m [us] to jest
}
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************************
//*****************************************************************************************
//**********************U S A R T _ FUNKCJE OBSLUGI****************************************
//*****************************************************************************************
//*****************************************************************************************
// inicjalizacja UART
void USART_Init(uint16_t baud) {
	/* Ustawienie prêdkoœci */
	UBRRH = (uint8_t) (baud >> 8);
	UBRRL = (uint8_t) baud;
	/* Za³¹czenie nadajnika i odbiornika */
	UCSRB = (1 << RXEN) | (1 << TXEN);
	/* Ustawienie format ramki: 8bitów danych, 1 bit stopu */
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

// przes³anie ³añcucha znaków w poolingu (znak po znaku)
// bez buforowania - nie jest zwykle konieczne tak jak przy odbiorze
void uart_puts(char *s) // wysy³a ³añcuch z pamiêci RAM na UART
{
	register char c;
	while ((c = *s++))
		uart_putc(c); // dopóki nie napotkasz 0 wysy³aj znak
}

// wys³anie liczby na UART
void uart_put_int(uint16_t value) {
	char string[17]; // bufor na wynik funkcji itoa
	itoa(value, string, 10); // konwersja value na ASCII
	uart_puts(string); // wyœlij string na port szeregowy
}

// wys³anie pojedynczego znaku na UART (podstawowa funkcja robocza)
void uart_putc(char data) {
	/* Wait for empty transmit buffer */
	while (!(UCSRA & (1 << UDRE)))
		;
	/* Put data into buffer, sends the data */
	UDR = data;
}

// funkcja wysy³aj¹ca wszystkie zebrane czasy z tablicy do terminala
void send_to_terminal(uint16_t*tab) {
	uint8_t i;
	if (!pulse_cnt)
		return;
	uart_puts("ramka IR:\r\n");
	uart_puts("************************");
	uart_puts("\r\n");
	for (i = 0; i < ILOSC_KODOW; i++) {
		uart_puts("step: ");
		if (i + 1 < 10)
			uart_puts("0");
		uart_put_int(i + 1);
		uart_puts("\tPulseWidth: ");
		uart_put_int(((tab[i] * 1000UL) / ((F_CPU / 1000UL) / 8)));
		uart_puts("\r\n");
	}
	uart_puts("************************");
	uart_puts("\r\n");
}

//*****************************************************************************************
//*****************************************************************************************
//**********************U S A R T _ FUNKCJE OBSLUGI --- KONIEC****************************
//*****************************************************************************************
//*****************************************************************************************

