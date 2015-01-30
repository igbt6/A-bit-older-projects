/*
 * hd44780.c
 *
 *  Created on: 2011-07-10
 *       Autor: Miros≥aw Kardaú
 *       Poprawki: £ukasz Uszko
 */
//-----------------------------------------------------------------------------------------------------------
// *** Obs≥uga wyúwietlaczy alfanumerycznych zgodnych z HD44780 ***
//
// - Sterowanie: tryb 4-bitowy
// - Dowolne przypisanie kaødego sygna≥u sterujπcego do dowolnego pinu mikrokontrolera
// - Praca z pinem RW pod≥πczonym do GND lub do mikrokontrolera (sprawdzanie BusyFLAG - szybkie operacje LCD)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include "harddef.h"
#include "hd44780.h"
#include "pcf8583.h"

// makrodefinicje operacji na sygna≥ach sterujπcych RS,RW oraz E

#define SET_RS 	PORT(LCD_RSPORT) |= (1<<LCD_RS)			// stan wysoki na linii RS
#define CLR_RS 	PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		// stan niski na linii RS
#define SET_RW 	PORT(LCD_RWPORT) |= (1<<LCD_RW)			// stan wysoki na RW - odczyt z LCD
#define CLR_RW 	PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		// stan niski na RW - zapis do LCD
#define SET_E 	PORT(LCD_EPORT) |= (1<<LCD_E)			// stan wysoki na linii E
#define CLR_E 	PORT(LCD_EPORT) &= ~(1<<LCD_E)			// stan niski na linii E
uint8_t check_BF(void); // deklaracja funkcji wewnÍtrznej


//********************* FUNKCJE WEWN TRZNE *********************

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WYjúcia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_out(void) {
	DDR(LCD_D7PORT) |= (1 << LCD_D7);
	DDR(LCD_D6PORT) |= (1 << LCD_D6);
	DDR(LCD_D5PORT) |= (1 << LCD_D5);
	DDR(LCD_D4PORT) |= (1 << LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WEjúcia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_in(void) {
	DDR(LCD_D7PORT) &= ~(1 << LCD_D7);
	DDR(LCD_D6PORT) &= ~(1 << LCD_D6);
	DDR(LCD_D5PORT) &= ~(1 << LCD_D5);
	DDR(LCD_D4PORT) &= ~(1 << LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Wys≥anie po≥Ûwki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline void lcd_sendHalf(uint8_t data) {
	if (data & (1 << 0))
		PORT(LCD_D4PORT) |= (1 << LCD_D4);
	else
		PORT(LCD_D4PORT) &= ~(1 << LCD_D4);
	if (data & (1 << 1))
		PORT(LCD_D5PORT) |= (1 << LCD_D5);
	else
		PORT(LCD_D5PORT) &= ~(1 << LCD_D5);
	if (data & (1 << 2))
		PORT(LCD_D6PORT) |= (1 << LCD_D6);
	else
		PORT(LCD_D6PORT) &= ~(1 << LCD_D6);
	if (data & (1 << 3))
		PORT(LCD_D7PORT) |= (1 << LCD_D7);
	else
		PORT(LCD_D7PORT) &= ~(1 << LCD_D7);
}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt po≥Ûwki bajtu z LCD (D4..D7) w celu sprawdzenia flagi zajÍtoúci
//
//----------------------------------------------------------------------------------------
static inline uint8_t lcd_readHalf(void) {
	uint8_t result = 0;

	if (PIN(LCD_D4PORT) & (1 << LCD_D4))
		result |= (1 << 0);
	if (PIN(LCD_D5PORT) & (1 << LCD_D5))
		result |= (1 << 1);
	if (PIN(LCD_D6PORT) & (1 << LCD_D6))
		result |= (1 << 2);
	if (PIN(LCD_D7PORT) & (1 << LCD_D7))
		result |= (1 << 3);

	return result;
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wyúwietlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data) {
	// Ustawienie pinÛw portu LCD D4..D7 jako wyjúcia
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4); // wys≥anie starszej czÍúci bajtu danych D7..D4
	CLR_E;

	SET_E;
	lcd_sendHalf(_data); // wys≥anie m≥odszej czÍúci bajtu danych D3..D0
	CLR_E;

#if USE_RW == 1
	while ((check_BF() & (1 << 7)))
		; // sprawdzam flagÍ zajÍtoúci(Busy Flag), interesuje mnie tylko bit
#else                                   //najstarszy 1<<7
	_delay_us(120);
#endif

}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt bajtu z wyúwietlacza LCD
//
//----------------------------------------------------------------------------------------
uint8_t _lcd_read_byte(void) {
	uint8_t result = 0;
	data_dir_in();

	SET_RW;

	SET_E;
	result = (lcd_readHalf() << 4); // odczyt starszej czÍúci bajtu z LCD D7..D4
	CLR_E;

	SET_E;
	result |= lcd_readHalf(); // odczyt m≥odszej czÍúci bajtu z LCD D3..D0
	CLR_E;

	return result;
}
#endif

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Sprawdzenie stanu Busy Flag (ZajÍtoúci wyúwietlacza)
//
//----------------------------------------------------------------------------------------
uint8_t check_BF(void) //funkcja sprawdzajπca flagÍ zajÍtoúci
{ //zwraca ca≥y bajt
	CLR_RS;
	return _lcd_read_byte();
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wyúwietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd) {
	CLR_RS;
	_lcd_write_byte(cmd);
}

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wyúwietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data) {
	SET_RS;
	_lcd_write_byte(data);
}

//**************************  FUNKCJE PRZEZNACZONE TAKØE DLA INNYCH MODU£”W  ******************

#if USE_LCD_CHAR == 1
//----------------------------------------------------------------------------------------
//
//		 Wys≥anie pojedynczego znaku do wyúwietlacza LCD w postaci argumentu
//
//		 8 w≥asnych znakÛw zdefiniowanych w CGRAM
//		 wysy≥amy za pomocπ kodÛw 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_char(char c) {
	lcd_write_data((c >= 0x80 && c <= 0x87) ? (c & 0x07) : c);
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Wys≥anie stringa do wyúwietlacza LCD z pamiÍci RAM
//
//		 8 w≥asnych znakÛw zdefiniowanych w CGRAM
//		 wysy≥amy za pomocπ kodÛw 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_put_str(char * str) {
	register char znak;
	while ((znak = *(str++)))
		lcd_write_data((znak >= 0x80 && znak <= 0x87) ? (znak & 0x07) : znak);
}

#if USE_LCD_STR_P == 1
//----------------------------------------------------------------------------------------
//
//		 Wys≥anie stringa do wyúwietlacza LCD z pamiÍci FLASH
//
//		 8 w≥asnych znakÛw zdefiniowanych w CGRAM
//		 wysy≥amy za pomocπ kodÛw 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str_P(char * str) {
	register char znak;
	while ((znak = pgm_read_byte(str++)))
		lcd_write_data(((znak >= 0x80) && (znak <= 0x87)) ? (znak & 0x07)
				: znak);
}
#endif

#if USE_LCD_STR_E == 1
//----------------------------------------------------------------------------------------
//
//		 Wys≥anie stringa do wyúwietlacza LCD z pamiÍci EEPROM
//
//		 8 w≥asnych znakÛw zdefiniowanych w CGRAM
//		 wysy≥amy za pomocπ kodÛw 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str_E(char * str) {
	register char znak;
	while (1) {
		znak = eeprom_read_byte((uint8_t *) (str++));
		if (!znak || znak == 0xFF)
			break;
		else
			lcd_write_data(((znak >= 0x80) && (znak <= 0x87)) ? (znak & 0x07)
					: znak);
	}
}
#endif

#if USE_LCD_INT == 1
//----------------------------------------------------------------------------------------
//
//		 Wyúwietla liczbÍ dziesiÍtnπ na wyúwietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val) {
	char bufor[17];
	lcd_put_str(itoa(val, bufor, 10));
}
#endif

#if USE_LCD_HEX == 1
//----------------------------------------------------------------------------------------
//
//		 Wyúwietla liczbÍ szestnastkowπ HEX na wyúwietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(int val) {
	char bufor[17];
	lcd_put_str(itoa(val, bufor, 16));
}
#endif

#if USE_LCD_DEFCHAR == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w≥asnego znaku na LCD z pamiÍci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamiÍci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaünik do tablicy 7 bajtÛw definiujπcych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i, c;
	lcd_write_cmd(64 + ((nr & 0x07) * 8));
	for (i = 0; i < 8; i++) {
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_P == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w≥asnego znaku na LCD z pamiÍci FLASH
//
//		argumenty:
//		nr: 		- kod znaku w pamiÍci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaünik do tablicy 7 bajtÛw definiujπcych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_P(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i, c;
	lcd_write_cmd(64 + ((nr & 0x07) * 8));
	for (i = 0; i < 8; i++) {
		c = pgm_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_E == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w≥asnego znaku na LCD z pamiÍci EEPROM
//
//		argumenty:
//		nr: 		- kod znaku w pamiÍci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaünik do tablicy 7 bajtÛw definiujπcych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i, c;

	lcd_write_cmd(64 + ((nr & 0x07) * 8));
	for (i = 0; i < 8; i++) {
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_LOCATE == 1
//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zaleønoúci od rodzaju wyúwietlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_locate(uint8_t y, uint8_t x) {
	switch (y) {
	case 0:
		y = LCD_LINE1;
		break;

#if (LCD_Y>1)
	case 1:
		y = LCD_LINE2;
		break; // adres 1 znaku 2 wiersza
#endif
#if (LCD_Y>2)
		case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
#endif
#if (LCD_Y>3)
		case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
#endif
	}

	lcd_write_cmd((0x80 + y + x));
}
#endif

//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyúwietlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void) {
	lcd_write_cmd(LCDC_CLS);

#if USE_RW == 0
	_delay_ms(4.9);
#endif
}

#if USE_LCD_CURSOR_HOME == 1
//----------------------------------------------------------------------------------------
//
//		PowrÛt kursora na poczπtek
//
//----------------------------------------------------------------------------------------
void lcd_home(void) {
	lcd_write_cmd(LCDC_CLS | LCDC_HOME);

#if USE_RW == 0
	_delay_ms(4.9);
#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1
//----------------------------------------------------------------------------------------
//
//		W≥πczenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void) {
	lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON | LCDC_CURSORON);
}

//----------------------------------------------------------------------------------------
//
//		Wy≥πczenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_off(void) {
	lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
}
#endif

#if USE_LCD_CURSOR_BLINK == 1
//----------------------------------------------------------------------------------------
//
//		W£πcza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void) {
	lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON | LCDC_CURSORON | LCDC_BLINKON);
}

//----------------------------------------------------------------------------------------
//
//		WY≥πcza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_off(void) {
	lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
}
#endif
//----------------------------------------------------------------------------------------------------
// Definicja znakÛw narodowych
// (Polski ma ich 18)
/*
 *
 prog_char PLlcdspec[18] =
 {
 // Znaczki - poprawi≥em pod wzglÍdem wyglπdu znaczki z EdW 1/98, doda≥em brakujπce
 {{0, 0, 14, 1, 15, 17, 15, 2}, 'a'},    //π - kod 0x80
 {{2, 4, 14, 16, 16, 17, 14, 0}, 'c'},   //Ê - kod 0x81
 {{0, 0, 14, 17, 31, 16, 14, 2}, 'e'},   //Í - kod 0x82
 {{12, 4, 6, 4, 12, 4, 14, 0}, 'l'},     //≥ - kod 0x83
 {{2, 4, 22, 25, 17, 17, 17, 0}, 'n'},   //Ò - kod 0x84
 {{2, 4, 14, 17, 17, 17, 14, 0}, 'o'},   //Û - kod 0x85
 {{2, 4, 14, 16, 14, 1, 30, 0}, 's'},    //ú - kod 0x86
 {{2, 4, 31, 2, 4, 8, 31, 0}, 'z'},      //ü - kod 0x87
 {{4, 0, 31, 2, 4, 8, 31, 0}, 'z'},      //ø - kod 0x88
 {{14, 17, 17, 31, 17, 17, 17, 2}, 'A'}, //• - kod 0x89
 {{2, 14, 21, 16, 16, 16, 17, 14}, 'C'}, //∆ - kod 0x8a
 {{31, 16, 16, 30, 16, 16, 31, 2}, 'E'}, //  - kod 0x8b
 {{16, 16, 20, 24, 16, 16, 31, 0}, 'L'}, //£ - kod 0x8c
 {{2, 21, 17, 25, 21, 19, 17, 17}, 'N'}, //— - kod 0x8d
 {{2, 14, 21, 17, 17, 17, 17, 14}, 'O'}, //” - kod 0x8e
 {{2, 15, 20, 16, 14, 1, 1, 30}, 'S'},   //å - kod 0x8f
 {{2, 31, 5, 2, 4, 8, 16, 31}, 'Z'},     //è - kod 0x90
 {{4, 31, 1, 2, 4, 8, 16, 31}, 'Z'},     //Ø - kod 0x91
 };
*/
// Teksty
/*prog_char PL_strDisplay[] = "Polski";
prog_char PL_strStart[] = " Witaj w wersji     Polskiej";
prog_char PL_strCnt[] = "Licznik: %d";
prog_char PL_strHold[] = "Przytrzymano: %d";
prog_char PL_strLight[] = "Jasno" "\x86" "\x81" ": %d";
prog_char PL_strPos1[] = "Pozycja 1";
prog_char PL_strPos2[] = "Pozycja 2";
prog_char PL_strPos3[] = "Pozycja 3";
prog_char PL_strPos4[] = "Pozycja 4";
prog_char PL_strPos5[] = "Pozycja 5";
prog_char PL_str2Sub[] = "Do podmenu...";
prog_char PL_strSub1[] = "Podmenu 1";
prog_char PL_strSub2[] = "Podmenu 2";
prog_char PL_strSub3[] = "Podmenu 3";
prog_char PL_strSub4[] = "Podmenu 4";
prog_char PL_strSub5[] = "Podmenu 5";
prog_char PL_strSub6[] = "Podmenu 6";
prog_char PL_strSub7[] = "Podmenu 7";
prog_char PL_strSub8[] = "Podmenu 8";
prog_char PL_strProgramEnd[] = "Koniec programu";*/
prog_char PL_tab1[]={"LUKASZ_USZKO"};
prog_char PL_tab2[]={"TAK          NIE"};
prog_char PL_cont[]={"Kontynuowac?"};
prog_char PL_save[]={"Zapisac zmiany?"};
prog_char PL_error[]={"ERROR!"};
prog_char PL_powitanie[]={"UNIWERSALNY STEROWNIK"};
prog_char PL_oprog[]={"OPROGRAMOWANIE:"};
prog_char PL_next_alarm[]={"ust. nast. alarm"};
prog_char PL_wersja[]={"wersja 1.01"};
//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WYåWIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void) {
	// inicjowanie pinÛw portÛw ustalonych do pod≥πczenia z wyúwietlaczem LCD
	// ustawienie wszystkich jako wyjúcia
	data_dir_out();
	DDR(LCD_RSPORT) |= (1 << LCD_RS);
	DDR(LCD_EPORT) |= (1 << LCD_E);
#if USE_RW == 1
	DDR(LCD_RWPORT) |= (1 << LCD_RW);
#endif

	// wyzerowanie wszystkich linii sterujπcych
	PORT(LCD_RSPORT) &= ~(1 << LCD_RS);
	PORT(LCD_EPORT) &= ~(1 << LCD_E);
#if USE_RW == 1
	PORT(LCD_RWPORT) &= ~(1 << LCD_RW);
#endif

	_delay_ms(15);
	PORT(LCD_RSPORT) &= ~(1 << LCD_RS);
	PORT(LCD_RWPORT) &= ~(1 << LCD_RW);

	// jeszcze nie moøna uøywaÊ Busy Flag
	lcd_sendHalf(LCDC_FUNC | LCDC_FUNC8B);
	_delay_ms(4.1);
	lcd_sendHalf(LCDC_FUNC | LCDC_FUNC8B);
	_delay_us(100);
	lcd_sendHalf(LCDC_FUNC | LCDC_FUNC4B);
	_delay_us(100);

	// juø moøna uøywaÊ Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd(LCDC_FUNC | LCDC_FUNC4B | LCDC_FUNC2L | LCDC_FUNC5x7);
	// wy≥πczenie kursora
	lcd_write_cmd(LCDC_ONOFF | LCDC_CURSOROFF);
	// w≥πczenie wyúwietlacza
	lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
	// przesuwanie kursora w prawo bez przesuwania zawartoúci ekranu
	lcd_write_cmd(LCDC_ENTRY | LCDC_ENTRYR);

	// kasowanie ekranu
	lcd_cls();
}
//----------------------------------------------------------------------------------------
//
//		Funkcja przesuwajπca obraz na wyswietlaczu od lewej do prawej i z powrotem
//      w obu wierszach
//      nie planuje w obu wierszach d≥uøszych tekstÛw niø 16 znakÛw , jak cos to tylko gÛrny
//----------------------------------------------------------------------------------------
void move_char_on_lcd(prog_char* tab1, prog_char* tab2) {
	static uint8_t index1, index2;

	char tabl1[40];//
	char tabl2[40];
	char*wsk_tabl1 = &tabl1[0];
	//char*wsk_tabl2 = &tabl2[0];

	while (*tab1 != 0) {
		tabl1[index1] = *tab1++;
		index1++;
	}
	while (*tab2 != 0) {
		tabl2[index2] = *tab2++;
		index2++;
	}

	if (index1 < 16 && index2 < 16) { //sprawdzam czy d≥ugoúÊ ≥aÒcucha mniejsza od 16(d≥ ,wysw)
		uint8_t wymiar1;
		uint8_t wymiar2;
		uint8_t rozn_wym;
		wymiar2 = 16 - index2; //licze d≥ugoúÊ wolnego miejsca na wyswietlaczu
		wymiar1 = 16 - index1; //licze d≥ugoúÊ wolnego miejsca na wyswietlaczu
		rozn_wym = (index1 >= index2 ? wymiar1 : wymiar2); // jeúli wym1>= wym2 to rozn_wym=wym1
		while (1) {
			for (uint8_t i = 0; i != rozn_wym; i++) {
				if (index1 != 0) {
					lcd_locate(0, i);
					lcd_put_str(tabl1);

				} else if (index2 != 0) {
					lcd_locate(1, i); // jedziemy po ca≥ym wyswietlaczu
					lcd_put_str(tabl2);
				}
				Delay_100ms(5);

				lcd_cls();
			}
			for (uint8_t i = rozn_wym; i != 0; i--) {
				if (index1 != 0) {
					lcd_locate(0, i);
					lcd_put_str(tabl1);
				} else if (index2 != 0) {
					lcd_locate(1, i); // jedziemy po ca≥ym wyswietlaczu
					lcd_put_str(tabl2);
				}
				Delay_ms(500);

				lcd_cls();
			}

		}
	} else if (index1 >= 16) /*&& (index2 < 16)*/{

		uint8_t wymiar_to_high;
		wymiar_to_high = abs(15-index1); // wyliczam o ile jest za d≥ugi ciπg wyrazÛw// potrzebne mi to do okreúlenia kiedy zawijaÊ od nowa drugi wyraz
		while (1) {

			for (uint8_t i = 0; i != index1 + 4; i++) { // +4 bo musze zachowaÊ miejsce na drugπ wiad
				if (*wsk_tabl1 != 0) {
					lcd_locate(0, 0);
					lcd_put_str(wsk_tabl1); // taki myk øe co pÍtla w zerze wyswietlam kolejny znak z tablicy
					wsk_tabl1++;
				}
				if (index2 != 0) {

					lcd_locate(1, 0);
					lcd_put_str(tabl2);

				}

				if (i >= (wymiar_to_high)) { //od koÒca wysw 3 gwiazdki øeby oddzieliÊ tekst
					static uint8_t k = 19;
					switch (k) {
					case 3: {
						lcd_locate(0, 0);
						lcd_put_str("**");
						k--;
						break;
					}
					case 2: {
						lcd_locate(0, 0);
						lcd_put_str("*");
						k--;

						break;
					}
					case 1: {
						//	lcd_cls();
						k--;
						break;
					}
					case 0: {
						lcd_cls();
				//	k = 19;
						break;
					}
					default: {
						lcd_locate(0, k-4);
						lcd_put_str("***");
						k--;
						break;
					}

					}
					if (i >= (wymiar_to_high) && i < wymiar_to_high + 3) {
						Delay_ms(250);
						lcd_cls();
					} else {
						static uint8_t j = 16;
						lcd_locate(0, j - 1);
						lcd_put_str(tabl1);
						j--;
						Delay_ms(250);
						lcd_cls();
						if (j == 0) {
							j = 16;
							k = 19;
							wsk_tabl1 = &tabl1[0];
							continue;
						}

					}
				}

				else {
					Delay_ms(250);
					lcd_cls();
				}
			}

		}
	}
}

