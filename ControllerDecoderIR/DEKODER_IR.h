/*
 * DEKODER_IR.h
 *
 *  Created on: 2012-04-18
 *       Autor: £ukasz Uszko
 */
#include "stdbool.h"
#include <avr/eeprom.h>
#ifndef DEKODER_IR_H_
#define DEKODER_IR_H_

//**************************************************
#define ILOSC_KODOW 85 // DLA PROCESOROW z wiekszym eepromem dac mo¿na 100
#define TOLERANCE 250   // tolerancja czasow , odebranych ramek
#define ir_micro_s(num) (((num)*((F_CPU/1000UL)/8))/1000)
#define UART_BAUD 38400		// tu definiujemy interesuj¹c¹ nas prêdkoœæ
#define __UBRR F_CPU/16/UART_BAUD-1  // obliczamy UBRR dla U2X=0

#define IR_PIN (1<<PB0)			// numer pinu wejœcia ICP
#define IR_DIR DDRB
#define IR_PORT PORTB
#define IR_ON  TIMSK|=(1 << TICIE1)            //w³¹czam zezwolenie na przerwania od ICP1
#define IR_OFF 	TIMSK&=~(1 << TICIE1)              //wy³¹czam zezwolenie na przerwania od ICP

// deklaracje funkcji
void USART_Init(uint16_t baud); // obs³uga UART
void uart_puts(char *s); // obs³uga UART
void uart_putc(char data); // obs³uga UART
void uart_put_int(uint16_t value); // obs³uga UART
void send_to_terminal( uint16_t*);

enum flaga_przycisku{pierwszy=1, drugi, trzeci};
volatile enum flaga_przycisku Which_Key_is_Pressed; // flaga który switch programuje
//volatile bool Which_Frame_is_Programming;

//definicje funkcji
void Programing_NewPilot_IR(void) ; //bede zwraca³ wartoœæ zale¿nie czy zapisano w eepromie czy nie
void Conf_and_Wait_for_Frame(void);  // funkcja u¿ywana w funkcji programuj¹cej;
 uint16_t* Copy_Tab(uint16_t* tab_to_fill,volatile uint16_t* tab_with_frame, uint8_t _size);// funkcja kopiuj¹ca tablice odebrana w przerwaniu do nowej
bool Compare_2_Tab(uint16_t* tab1,uint16_t* tab2, uint8_t _size);
void Clear_3_Tab(uint16_t* tab1, uint16_t* tab2,volatile uint16_t* tab_ICP, uint8_t _size); // funkcja zeruj¹ca dwie tablice globalne u¿ywane w programowaniu
void Clear_1_Tab(volatile uint16_t* tab_ICP, uint8_t _size); //funkcja uzywana w mainie tablice ICP po kazdym odebraniu ramki (musze uzyc bo uzywam toggle bita iby mi zostawaly flaki w tab
void Confirmation_Successful_Write_to_EEPROM(void); // funkcje s³u¿¹ce do zamrugania dioda w celu potwierdzenia
void Confirmation_Unsuccessful_Write_to_EEPROM(void);//udanego badz nieudanego zapisu do eepromu (nie musi to byæ dida to zale¿y kiedy i jak kto bedzie ta biblioteke uzywal)
void Init_Timer1(void); // inicjalizacja timera od przerwani ICP obslugujacego podczerwien
// definicje zmiennych
volatile uint16_t tab[ILOSC_KODOW]; // tablica przechowuj¹ca ramkê w przerwaniu
volatile bool frame_end;
volatile uint8_t step, pulse_cnt;
uint16_t tab_in_eeprom[ILOSC_KODOW]; // tablica do której bede zczytywal dane z eeproma i porownywal z odebranymi
uint16_t  Tab_Code_EEPROM_IR_1[ILOSC_KODOW]; // adresy eeprom
uint16_t  Tab_Code_EEPROM_IR_2[ILOSC_KODOW];
uint16_t  Tab_Code_EEPROM_IR_3[ILOSC_KODOW];



#endif /* DEKODER_IR_H_ */
