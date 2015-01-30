/*
 * main.c
 *
 *  Created on: 04-12-2012
 *      Author: Lukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lcd44780.h"
#include "ds1621.h"
#include "i2c_twi.h"
#include <stdlib.h> //dla itoa


#define UART_BAUD 38400		// tu definiujemy interesuj¹c¹ nas prêdkoœæ UARTa
#define __UBRR F_CPU/16/UART_BAUD-1  // obliczamy UBRR dla U2X=0

// deklaracje funkcji
void USART_Init(uint16_t baud); // init UART
void uart_puts(char *s); // wysylanie stringa
void uart_putc(char data); // wysylanie znaku
void uart_put_int(uint16_t value); // wysylanie liczby


volatile uint8_t flag_czuj = 0;

void TIMER0_Conf(void);

int main(void) {
	//	USART_Init(__UBRR); // inicjalizacja UART to se odblokniesz jak bedziesz chcial uzyc
	unsigned char* tempDisplay;
	//TIMER0_Conf();
	lcd_init();
	lcd_locate(0, 0);
			lcd_str("T1:");
			DDRA|=0xFF;
			PORTA=0xFF;
#if(0)
			if (ds1621_init(DS1621_addrW_NO1)) {
		lcd_locate(0, 0);
		lcd_str("T1:");
		lcd_str((char*)READ_TEMP_ERROR);
	}
	if (ds1621_init(DS1621_addrW_NO2)) {
		lcd_locate(1, 0);
		lcd_str("T2:");
		lcd_str((char*)READ_TEMP_ERROR);
	}
	sei();
	// enable interrupts
	while (1) {
		switch (flag_czuj) {

		case 0:
			break; // mozna to bardziej optymalnie niz switchem ale to juz do zabawy zostawiam
		case 1: {
			tempDisplay = getTemperature(DS1621_addrW_NO1);
			lcd_locate(0, 0);
			lcd_str("T1:");
			lcd_str((char*) tempDisplay);
			flag_czuj = 0;
			break;
		}

		case 2: {
			tempDisplay = getTemperature(DS1621_addrW_NO2);
			lcd_locate(1, 0);
			lcd_str("T2:");
			lcd_str((char*) tempDisplay);
			flag_czuj = 0;
			break;
		}
		}
	};
#endif
	while(1);
	return 0;
}



//*****************************************************************************************
//*****************************************************************************************
//**********************USART 0  _ FUNKCJE OBSLUGI****************************************
//*****************************************************************************************
//*****************************************************************************************
// inicjalizacja UART
void USART_Init(uint16_t baud) {
	/* Ustawienie prêdkoœci */
	UBRR0H = (uint8_t) (baud >> 8);
	UBRR0L = (uint8_t) baud;
	/* Za³¹czenie nadajnika i odbiornika */
	UCSR0B = (1 << RXEN) | (1 << TXEN);
	/* Ustawienie format ramki: 8bitów danych, 1 bit stopu */
	UCSR0C = (1 << UMSEL0) | (3 << UCSZ00);
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
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

//*****************************************************************************************
//*****************************************************************************************
//**********************U S A R T _ FUNKCJE OBSLUGI --- KONIEC****************************
//*****************************************************************************************
//*****************************************************************************************
