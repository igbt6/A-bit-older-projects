/*
 * pcf8583.c
 *
 *  Created on: 2012-02-21
 *       Autor: £ukasz Uszko
 */
#include <avr/io.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c_twi.h"
#include "pcf8583.h"
#include "hd44780.h"
#include "harddef.h"


/***************************************************************************************************************
 * MOJE funkcje do obs³ugi PCF8583
 */////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/**************************************************************************************************************/

/***************************************************************************************************************
 * Funkcje konwertuj¹ce  DEC->BCD i odwrotnie
 * @param dec -sta³a 1 bajt bez znaku
 * @return sta³a 1 bajt bez znaku
 *//***********************************************************************************************************/

uint8_t dec_to_bcd(uint8_t dec) {
	return ((dec / 10) << 4) | (dec % 10);
}

uint8_t bcd_to_dec(uint8_t bcd) {
	return ((((bcd >> 4) & 0x0F) * 10) + (bcd & 0x0F));
}
/**************************************************************************************************************
 * ///funkcja zapisu tablicy do zegara , w kodzie BCD
 * @param tabl_czas
 *//***********************************************************************************************************/

void write_time(uint8_t *tabl_czas) {

	uint8_t i = 0;
	uint8_t tab[6] = { 0, 0, 0, 0, 0, 0 };
	lcd_cls();
	while (i < 6) {

		if (i == 4) {
			tab[i] |= (dec_to_bcd((tabl_czas[i] >> 6) & ~(0xC0))) << 6; //RTC_year
			tab[i] |= (dec_to_bcd(tabl_czas[i] & ~(0xC0))); // day_month

		} else if (i == 5) {
			tab[i] |= (dec_to_bcd((tabl_czas[i] >> 5) & ~(0xE0))) << 5; //dni tygodnia
			tab[i] |= (dec_to_bcd(tabl_czas[i] & ~(0xE0))); //miesi¹ce

		} else
			tab[i] = dec_to_bcd(tabl_czas[i]);
		i++;
	}
	I2C_write_data(PCF8583_ADDR, 0x01, 6, tab);
	write_time_year(&Rtc_Parametry);
}
/****************************************************************************************************************
 * Funkcja zapisu elementu struktur year pod pierwsze dwa bajty 0x10 i 0x11 pamiêci RAM zegara RTC
 * @param wsk_on_struct_RTC_TIME
 *//************************************************************************************************************/
void write_time_year(RTC_TIME*wsk_on_struct_RTC_TIME) { //funkcja zapisu
	uint8_t tab_year[2] = { dec_to_bcd(((wsk_on_struct_RTC_TIME)->year )/ 100),
			dec_to_bcd((wsk_on_struct_RTC_TIME->year) % 100) }; //muszê podzieliæ rok na 2 gdy¿ zajmuje on 2 bajty
	I2C_write_data(PCF8583_ADDR, 0x10, 2, tab_year);
}
/**************************************************************************************************************
 * ///funkcja odczytu tablicy do zegara , w kodzie BCD //i zwróci mi wskaxnik do strukturki Rtc_Parametry
 * @return
 *//***********************************************************************************************************/

RTC_TIME* read_time(void) {
	uint8_t i = 0;
	uint8_t tab_year[2] = { 0, 0 };
	I2C_read_data(PCF8583_ADDR, 0x10, 2, tab_year); //odczytuje rok z pamieci
	uint8_t tabl_czas[6] = { 0, 0, 0, 0, 0, 0 }; // tworzê tablicê czas
	uint8_t tabl_czas_bufor[8] = { 0, 0, 0, 0, 0, 0 }; //tablica bufor ,Ÿeby sie mi nie nadpisywa³y dane w tamtej
	I2C_read_data(PCF8583_ADDR, 0x01, 6, tabl_czas);
	while (i < 6) {
		if (i == 4) { //dni miesiaca , rok
			tabl_czas_bufor[i] |= (bcd_to_dec((tabl_czas[i] >> 6) & (0x03))) //muszê te dane po umiesczaæ na swoim miejscu
					<< 6; // taki hard ¿eby to dzia³a³o
			tabl_czas_bufor[i] |= (bcd_to_dec(tabl_czas[i] & ~(0xC0)));
			//& ~(0xC0);

		} else if (i == 5) { // miesi¹ce , dni tygodnia
			tabl_czas_bufor[i] |= (bcd_to_dec((tabl_czas[i] >> 5) & (0x07)))
					<< 5;
			tabl_czas_bufor[i] |= (bcd_to_dec(tabl_czas[i] & ~(0xE0)))
			/*& ~(0xE0)*/;

		} else
			tabl_czas_bufor[i] = bcd_to_dec(tabl_czas[i]); // ³aduje ponownie t¹ tablicê wartosciami dziesiêtnymi
		i++;
	}
	tabl_czas_bufor[6] = bcd_to_dec(tab_year[0]); // tysi¹ce i setki        //czytam rok
	tabl_czas_bufor[7] = bcd_to_dec(tab_year[1]); //dziesiatki i jednoœci
	return tab_RTC_on_struct(tabl_czas_bufor, &Rtc_Parametry);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//funkcje konwertujace strukturê czas do tablicy która bedzie wysy³ana do RTC i odwrotnie
//wykorzystywane w funkcji set time

uint8_t* struct_RTC_on_tab(RTC_TIME* str, uint8_t* tab_czasu) {
	for (int i = 0; i < 6; i++) {
		if (i == 1) {
			tab_czasu[i] = str->sec;
		} else if (i == 2) {
			tab_czasu[i] = str->min;
		} else if (i == 3) {
			tab_czasu[i] = str->hour;
		} else if (i == 4) {
			tab_czasu[i] = (((str->year_to_RTC) << 6) & (0xC0)); // lata w 6 i 7 bicie
			tab_czasu[i] |= (str->day_month/* & ~(0xC0)*/); //dni s¹ tylko w 6 pierwszych bitach

		} else if (i == 5) {
			tab_czasu[i] = (((str->day_week) << 5) & (0xE0));//dni tygodnia ostatnie 3 bity
			tab_czasu[i] |= (str->month/* & ~(0xE0)*/); //miesi¹ce na 5 bitach

		}
	}
	return &tab_czasu[0];
}
////////////////////////////////////////////////////////////////////////////////////////////////
RTC_TIME* tab_RTC_on_struct(uint8_t* tab_czasu, RTC_TIME* str) {

	for (int i = 0; i < 8; i++) {
		if (i == 1) {
			str->sec = tab_czasu[i];
		} else if (i == 2) {
			str->min = tab_czasu[i];
		} else if (i == 3) {
			str->hour = tab_czasu[i];
		} else if (i == 4) {
			str->day_month = (tab_czasu[i] & ~(0xC0)); //dni s¹ tylko w 6 pierwszych bitach
			str->year_to_RTC = (((tab_czasu[i]) >> 6) & (0x03)); // lata w 6 i 7 bicie
		} else if (i == 5) {
			str->month = (tab_czasu[i] & ~(0xE0)); //miesi¹ce na 5 bitach
			str->day_week = (((tab_czasu[i]) >> 5) & (0x07));//dni tygodnia ostatnie 3 bity
		} else if (i == 6) {
			str->year = tab_czasu[i] * 100;
		} else if (i == 7) {
			str->year += tab_czasu[i]; //tutaj sumuje oba bajty w jeden ¿eby wyznaczyæ rok
		}
	}
	return str;
}

/*	 RTC_TIME tab_RTC_on_struct_wart(uint8_t* tab_czasu,  RTC_TIME str) {

 for (int i = 0; i < 6; i++) {
 if (i == 1) {
 Rtc_Parametry.sec = tab_czasu[i];
 } else if (i == 2) {
 Rtc_Parametry.min = tab_czasu[i];
 } else if (i == 3) {
 Rtc_Parametry.hour = tab_czasu[i];
 } else if (i == 4) {
 Rtc_Parametry.day_month = (tab_czasu[i] & ~(0xC0)); //dni s¹ tylko w 6 pierwszych bitach
 Rtc_Parametry.year = (tab_czasu[i] & (0xC0)); // lata w 6 i 7 bicie
 } else if (i == 5) {
 Rtc_Parametry.month = (tab_czasu[i] & ~(0xE0)); //miesi¹ce na 5 bitach
 Rtc_Parametry.day_week = (tab_czasu[i] & (0xE0));//dni tygodnia ostatnie 3 bity
 }

 }return str;
 }*/
/////////////////////////////////////////////////////////////////////////////////////////////////
/*   //przez referencjê nie dzia³a -->>czemu nie wiem
 void tab_RTC_on_struct_ref(uint8_t*tab_czasu , RTC_TIME &str ){for (int i = 0; i < 6; i++) {
 if (i == 1) {
 str.sec = tab_czasu[i];
 } else if (i == 2) {
 str.min = tab_czasu[i];
 } else if (i == 3) {
 str.hour = tab_czasu[i];
 } else if (i == 4) {
 str.day_month = (tab_czasu[i] & ~(0xC0)); //dni s¹ tylko w 6 pierwszych bitach
 str.year = (tab_czasu[i] & (0xC0)); // lata w 6 i 7 bicie
 } else if (i == 5) {
 str.month = (tab_czasu[i] & ~(0xE0)); //miesi¹ce na 5 bitach
 str.day_week = (tab_czasu[i] & (0xE0));//dni tygodnia ostatnie 3 bity
 }

 }
 }
 */
//////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************
 * //funkcja wyswietlaj¹ca zmieniane wartoœci bez migotania
 * @param cursor
 *//***************************************************************************************/

void show_set_time(uint8_t cursor) {
	//static bool flaga_blink;


	lcd_cls();
	lcd_locate(0, 4);

	//if ( cursor == 0&&flaga_blink==0) {

	//	lcd_put_str("  ");
	//	Delay_100ms(4);
	//	flaga_blink = 1;
	//	} else if ((flaga_blink == 1&&cursor == 0)||cursor==1||cursor==2||cursor==3||cursor==4||cursor==5) {
	if (Rtc_Parametry.hour < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.hour);
		//	Delay_100ms(4);
		//flaga_blink = 0;

	} else {
		lcd_int(Rtc_Parametry.hour);
		//Delay_100ms(4);
		//flaga_blink = 0;

		//	}
	}

	lcd_put_str(":");
	if (Rtc_Parametry.min < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.min);
	} else {
		lcd_int(Rtc_Parametry.min);
	}

	lcd_put_str(":");
	if (Rtc_Parametry.sec < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.sec);
	} else {
		lcd_int(Rtc_Parametry.sec);
	}
	lcd_locate(1, 3);
	if (Rtc_Parametry.day_month < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.day_month);
	} else {
		lcd_int(Rtc_Parametry.day_month);
	}
	lcd_put_str("-");
	if (Rtc_Parametry.month < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.month);
	} else {
		lcd_int(Rtc_Parametry.month);
	}
	lcd_put_str("-");
	lcd_int(Rtc_Parametry.year);

	Delay_100ms(2);
}
/******************************************************************************************************
 * funkcja wyswietlajaca zegar i kalendarz
 *//**************************************************************************************************/

void show_time(void) {//lcd_cls();
	uint8_t after_read_year_to_RTC = Rtc_Parametry.year_to_RTC; // tutaj sprawdzam wartoœæ year_to_RTC przed odczytaniem i
	read_time();
	if (Rtc_Parametry.year_to_RTC > after_read_year_to_RTC ) {
		Rtc_Parametry.year++;
		write_time_year(&Rtc_Parametry);// zapisujê now¹ wartoœæ
	} //a tutaj po odczytaniu jeœli wzros³a po , to znaczy ¿e trzeba zmieniæ rok.

	lcd_locate(0, 4);
	if (Rtc_Parametry.hour < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.hour);
	} else {
		lcd_int(Rtc_Parametry.hour);
	}
	lcd_put_str(":");
	if (Rtc_Parametry.min < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.min);
	} else {
		lcd_int(Rtc_Parametry.min);
	}
	lcd_put_str(":");
	if (Rtc_Parametry.sec < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.sec);
	} else {
		lcd_int(Rtc_Parametry.sec);
	}
	lcd_locate(1, 3);
	if (Rtc_Parametry.day_month < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.day_month);
	} else {
		lcd_int(Rtc_Parametry.day_month);
	}
	lcd_put_str("-");
	if (Rtc_Parametry.month < 10) {
		lcd_put_str("0");
		lcd_int(Rtc_Parametry.month);
	} else {
		lcd_int(Rtc_Parametry.month);
	}
	lcd_put_str("-");
	lcd_int(Rtc_Parametry.year);

}

/******************************************************************************************************/
/*******************************************************************************************************
 *bool set_time(void) funkcja s³u¿¹ca do ustawienia zegara i kalendzarza
 * @return
 *//****************************************************************************************************/
/******************************************************************************************************/

bool set_time(void) {

	bool flaga_switch = 0; // bde u¿ywa³ tej flagi do ustawieñ switchami //wyskoki z pêtli zatrzymuj¹cych
	lcd_cls();
	uint8_t cursor = 0;//zmienna informuj¹ca o po³ozeniu kursora na ekranie //mówi mi co bede zmienia³//hour:min:sec
	while (1) {

		while (flaga_switch == 0) {
			lcd_locate(0, 2);
			lcd_str_P(PL_cont);
			if (Read_Input_Switch(SWITCH_LEWO) == true) {
				lcd_cls();
				flaga_switch = 1;
			} //jeœli  przycisk TAK wciœniêty to kontynuuj

			else if (Read_Input_Switch(SWITCH_PRAWO) == true) {
				lcd_cls();
				return false;
				flaga_switch = 1;
			}
		}
		uint8_t i = 0;
		uint8_t tab_czas[8];

		//if (licznik == first_settings) {
		while (i < 8) {
			tab_czas[i] = 1;

			i++;
		}
		show_set_time(cursor); //funkcja wyswietlaj¹ca zmieniaj¹c¹ siê strukturê Rtc_Parametry


		while (!Read_Input_Switch(SWITCH_OK)) {
			switch (cursor) {
			case 0: {//GODZINY
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ

				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.hour++; // tu mo¿na kiedys wyzmieniaæ ¿eby odnosiæ sie przez wskaŸnik ale narazie nie pali sie i nima po co
					if (Rtc_Parametry.hour > 23) {
						Rtc_Parametry.hour = 0;
					}
					show_set_time(cursor);
				} else if (Read_Input_Switch(SWITCH_DOL)) {
					//PORTB |= 1 << PB4;
					Rtc_Parametry.hour--;
					if (Rtc_Parametry.hour < 0) {
						Rtc_Parametry.hour = 23;
					}
					show_set_time(cursor);
				}
				if (Read_Input_Switch(SWITCH_PRAWO)) {
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 1;
				} else if (Read_Input_Switch(SWITCH_LEWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 5;
				}

				/*else if (Read_Input_Switch(SWITCH_OK)) {       ////DOKONCZYÆ!!! zakoñczenie ustawiania czasu--> pytanie o zapis
				 lcd_str_P(PL_save);
				 if(Read_Input_Switch(SWITCH_LEWO)){

				 I2C_write_data(PCF8583_ADDR, 0x01, 6, tab);
				 }

				 }*/
				break;
			}
			case 1: {//MINUTY
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.min++;
					if (Rtc_Parametry.min > 59) {
						Rtc_Parametry.min = 0;
					}
					show_set_time(cursor);
				} else if (Read_Input_Switch(SWITCH_DOL)) {
					//PORTB |= 1 << PB4;
					Rtc_Parametry.min--;
					if (Rtc_Parametry.min < 0) {
						Rtc_Parametry.min = 59;
					}
					show_set_time(cursor);
				}
				if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do godzin
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 0;
				} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 2;
				}
				break;

			}
			case 2: { //SEKUNDY
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.sec++;
					if (Rtc_Parametry.sec > 59) {
						Rtc_Parametry.sec = 0;
					}
					show_set_time(cursor);
				} else if (Read_Input_Switch(SWITCH_DOL)) {
					Rtc_Parametry.sec--;
					if (Rtc_Parametry.sec < 0) {
						Rtc_Parametry.sec = 59;
					}
					show_set_time(cursor);
				}
				if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do minut
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 1;
				} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 3;
				}
				break;
			}
			case 3: {//DNI MIESIACA
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
				czy_przestepny(&Rtc_Parametry); // liczê wartoœæ bitow¹ RTC_year, sprawdzam czy przestêpny i wpisuje
				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.day_month++;
					if ((Rtc_Parametry.month == 1 || Rtc_Parametry.month == 3
							|| Rtc_Parametry.month == 5 || Rtc_Parametry.month
							== 7 || Rtc_Parametry.month == 8
							|| Rtc_Parametry.month == 10 || Rtc_Parametry.month
							== 12) && (Rtc_Parametry.day_month > 31)) {
						Rtc_Parametry.day_month = 1;
					} else if ((Rtc_Parametry.month == 4 || Rtc_Parametry.month
							== 6 || Rtc_Parametry.month == 9
							|| Rtc_Parametry.month == 11)
							&& (Rtc_Parametry.day_month > 30)) {
						Rtc_Parametry.day_month = 1;
					} else if ((Rtc_Parametry.month == 2
							&& Rtc_Parametry.year_to_RTC == 0)
							&& (Rtc_Parametry.day_month > 29)) {
						Rtc_Parametry.day_month = 1;
					} else if ((Rtc_Parametry.month == 2
							&& (Rtc_Parametry.year_to_RTC == 1
									|| Rtc_Parametry.year_to_RTC == 2
									|| Rtc_Parametry.year_to_RTC == 3))
							&& (Rtc_Parametry.day_month > 28)) {
						Rtc_Parametry.day_month = 1;
					}
					show_set_time(cursor);
				} else if (Read_Input_Switch(SWITCH_DOL)) {

					Rtc_Parametry.day_month--;
					if (Rtc_Parametry.day_month < 1) {
						if (Rtc_Parametry.month == 1 || Rtc_Parametry.month
								== 3 || Rtc_Parametry.month == 5
								|| Rtc_Parametry.month == 7
								|| Rtc_Parametry.month == 8
								|| Rtc_Parametry.month == 10
								|| Rtc_Parametry.month == 12) {
							Rtc_Parametry.day_month = 31;
						} else if (Rtc_Parametry.month == 4
								|| Rtc_Parametry.month == 6
								|| Rtc_Parametry.month == 9
								|| Rtc_Parametry.month == 11) {
							Rtc_Parametry.day_month = 30;
						} else if (Rtc_Parametry.month == 2
								&& Rtc_Parametry.year_to_RTC == 0) {
							Rtc_Parametry.day_month = 29;
						} else if (Rtc_Parametry.month == 2
								&& (Rtc_Parametry.year_to_RTC == 1
										|| Rtc_Parametry.year_to_RTC == 2
										|| Rtc_Parametry.year_to_RTC == 3)) {
							Rtc_Parametry.day_month = 28;
						}
					}
					show_set_time(cursor);
				}
				if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do minut
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 2;
				} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 4;
				}
				break;

			}
			case 4: {//MIESI¥CE
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
				czy_przestepny(&Rtc_Parametry); // liczê wartoœæ bitow¹ RTC_year, sprawdzam czy przestêpny i wpisuje
				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.month++;
					if (Rtc_Parametry.month > 12) {
						Rtc_Parametry.month = 1;
					}
					//	show_set_time();
				} else if (Read_Input_Switch(SWITCH_DOL)) {
					Rtc_Parametry.month--;
					if (Rtc_Parametry.month < 1) {
						Rtc_Parametry.month = 12;
					}

				} else if ((Rtc_Parametry.month == 4 || Rtc_Parametry.month
						== 6 || Rtc_Parametry.month == 9 || Rtc_Parametry.month
						== 11) && (Rtc_Parametry.day_month > 30)) {
					Rtc_Parametry.day_month = 30;
				} else if ((Rtc_Parametry.month == 2
						&& Rtc_Parametry.year_to_RTC == 0)
						&& (Rtc_Parametry.day_month > 29)) {
					Rtc_Parametry.day_month = 29;
				} else if ((Rtc_Parametry.month == 2
						&& (Rtc_Parametry.year_to_RTC == 1
								|| Rtc_Parametry.year_to_RTC == 2
								|| Rtc_Parametry.year_to_RTC == 3))
						&& (Rtc_Parametry.day_month > 28)) {
					Rtc_Parametry.day_month = 28;
				}
				show_set_time(cursor);
				if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do minut
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 3;
				} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 5;
				}
				break;
			}
			case 5: {//ROK
				// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
				if (Read_Input_Switch(SWITCH_GORA)) {
					Rtc_Parametry.year++;
					if (Rtc_Parametry.year > 2100) {
						Rtc_Parametry.year = 2012;
					}
					czy_przestepny(&Rtc_Parametry); //sprawdzam tutaj przestêpnoœæ roku w celu zmiany 29 lutego na 28
					if (Rtc_Parametry.year_to_RTC != 0 && Rtc_Parametry.month
							== 2 && Rtc_Parametry.day_month == 29) {
						Rtc_Parametry.day_month = 28;
					}
					show_set_time(cursor);
				} else if (Read_Input_Switch(SWITCH_DOL)) {
					Rtc_Parametry.year--;
					if (Rtc_Parametry.year < 2012) {
						Rtc_Parametry.year = 2100;
					}
					czy_przestepny(&Rtc_Parametry); //sprawdzam tutaj przestêpnoœæ roku w celu zmiany 29 lutego na 28
					if (Rtc_Parametry.year_to_RTC != 0 && Rtc_Parametry.month
							== 2 && Rtc_Parametry.day_month == 29) {
						Rtc_Parametry.day_month = 28;
					}
					show_set_time(cursor);

				}
				if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do minut
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 4;
				} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
					Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
					cursor = 0;
				}
				break;
			}
			}
		}
		if (Read_Input_Switch(SWITCH_OK)) {
			lcd_cls();
			lcd_locate(0, 0);
			lcd_str_P(PL_save);
			while (1) {
				if (Read_Input_Switch(SWITCH_LEWO)) {
					czy_przestepny(&Rtc_Parametry); // liczê wartoœæ bitow¹ RTC_year, sprawdzam czy przestêpny i wpisuje
					dzien_tyg(&Rtc_Parametry); //licze dzien tygodnia i zapisuje go
					//Rtc_Parametry.day_week=6;
					write_time(struct_RTC_on_tab(&Rtc_Parametry, tab_czas));
					return true;
					break;
				} //czy zpisaæ dane? odp: tak
				else if (Read_Input_Switch(SWITCH_PRAWO)) {

					return false;
					break;//czy zpisaæ dane? odp: nie
				}
			}
		}

	}

}
/****************************************************************************************
 * funkcja sprawdzajaca czy rok jest przestêpny czy nie
 * @param wsk_struct_RTC_TIME
 * @return RTC_TIME*
 *//************************************************************************************/
void czy_przestepny(RTC_TIME *wsk_struct_RTC_TIME) {

	switch ((wsk_struct_RTC_TIME->year) % 4) {
	case 0: {
		wsk_struct_RTC_TIME->year_to_RTC = 0;
		//return wsk_struct_RTC_TIME;
		break;
	}
	case 1: {
		wsk_struct_RTC_TIME->year_to_RTC = 1;
		//return wsk_struct_RTC_TIME;
		break;
	}
	case 2: {
		wsk_struct_RTC_TIME->year_to_RTC = 2;
		//return wsk_struct_RTC_TIME;
		break;
	}
	case 3: {
		wsk_struct_RTC_TIME->year_to_RTC = 3;
		//return wsk_struct_RTC_TIME;
		break;
	}
	}
	//return 0;

}
/***************************************************************************************************************************************************
 * funkcja licz¹ca mi dzieñ roku  i ile dni pozosta³o do kóñca
 * @param wsk_struct_RTC_TIME
 * @return
 **************************************************************************************************************************************************/

void dzien_tyg(RTC_TIME *wsk_struct_RTC_TIME) { // funkcja liczaca  dzien tygodnia  /*tutaj zmieniæ ¿eby funkcja nie zwraca³a nic bo i poco
	wsk_struct_RTC_TIME->all_days = 0;   // tutaj zerujê co by sie mi nie zwieksza³o o 1 za du¿o
	if (wsk_struct_RTC_TIME->year_to_RTC == 0) {
		wsk_struct_RTC_TIME->days_in_year = 366;
	} else
		wsk_struct_RTC_TIME->days_in_year = 365;
	//if (wsk_struct_RTC_TIME->day_month == 1 && wsk_struct_RTC_TIME->month == 1&&wsk_struct_RTC_TIME->year==2012) {
	//wsk_struct_RTC_TIME->day_week = 6;
	//}

	if (wsk_struct_RTC_TIME->day_of_year > wsk_struct_RTC_TIME->days_in_year) {
		wsk_struct_RTC_TIME->day_of_year = 1;
		if (wsk_struct_RTC_TIME->year_to_RTC == 1) {
			wsk_struct_RTC_TIME->all_days += 366;
		} else
			wsk_struct_RTC_TIME->all_days += 365;
	} // zabezpieczenie przed  przekrtoczeniem dni

	for (uint8_t index = 1; index <= wsk_struct_RTC_TIME->month; ++index) {
		if (index == 1) {
			wsk_struct_RTC_TIME->day_of_year = (wsk_struct_RTC_TIME->day_month);

		} else if (index == 2 || index == 4 || index == 6 || index == 8 //tutaj dodajê po przekroczeniu miesiaca liczbê dni poprzedniego miesi¹ca
				|| index == 9 || index == 11) {
			wsk_struct_RTC_TIME->day_of_year += 31; // wszystko po to  ile bede dodawa³  w pêtli  w zale¿noœci od miesi¹ca

		} else if (index == 5 || index == 7 || index == 10 || index == 12) {
			wsk_struct_RTC_TIME->day_of_year += 30;

		} else if (index == 3 && wsk_struct_RTC_TIME->year_to_RTC == 0) {
			wsk_struct_RTC_TIME->day_of_year += 29;

		} else if ((index == 3) && ((wsk_struct_RTC_TIME->year_to_RTC == 1
				|| wsk_struct_RTC_TIME->year_to_RTC == 2
				|| wsk_struct_RTC_TIME->year_to_RTC == 3))) {
			wsk_struct_RTC_TIME->day_of_year += 28;
		}

	}
	wsk_struct_RTC_TIME->all_days += wsk_struct_RTC_TIME->day_of_year;
	if (wsk_struct_RTC_TIME->all_days < 7) {//czyli pierwsze 7 dni 2012 roku
		wsk_struct_RTC_TIME->day_week = wsk_struct_RTC_TIME->all_days % 7 - 1;
	}
	if (wsk_struct_RTC_TIME->all_days >= 7) {
		if (!(wsk_struct_RTC_TIME->all_days % 7)) {
			wsk_struct_RTC_TIME->day_week = 6;
		} else {
			wsk_struct_RTC_TIME->day_week = wsk_struct_RTC_TIME->all_days % 7
					- 1;
		}
	}
	// -1 ¿eby by³o od 0 do 6
}
/******************************************************************************************************
 * funkcja wyswietlajaca dzieñ tygodnia
 *//**************************************************************************************************/

void show_day_week(void) {
	uint8_t after_read_day_in_week = Rtc_Parametry.day_week; // tutaj sprawdzam wartoœæ dni_tygodnia przed odczytaniem i po
		read_time();
		if (Rtc_Parametry.day_week > after_read_day_in_week) {
			Rtc_Parametry.day_of_year++;
			Rtc_Parametry.all_days++;
			if (Rtc_Parametry.day_of_year > Rtc_Parametry.days_in_year) {
				Rtc_Parametry.day_of_year = 1;
					if (Rtc_Parametry.year_to_RTC == 1) {
						Rtc_Parametry.all_days += 366;
					} else
						Rtc_Parametry.all_days += 365;
				}
		} //a tutaj po odczytaniu jeœli wzros³a po , to znaczy ¿e trzeba zmieniæ rok.
	//dzien_tyg(&Rtc_Parametry); // to po to a¿eby mi siê zmienia³ te¿ dzieñ roku po zmianie na kolejny dzieñ tygodnia

lcd_locate(1, 0);
	lcd_put_str("> ");
	lcd_int(Rtc_Parametry.day_of_year);
	lcd_put_str(" dzien roku");

	lcd_locate(0, 0);

	switch (Rtc_Parametry.day_week) {
	case 0: {
		lcd_put_str("> NIEDZIELA");
		break;
	}
	case 1: {
		lcd_put_str("> PONIEDZIALEK");
		break;
	}
	case 2: {
		lcd_put_str("> WTOREK");
		break;
	}
	case 3: {
		lcd_put_str("> SRODA");
		break;
	}
	case 4: {
		lcd_put_str("> CZWARTEK");
		break;
	}
	case 5: {
		lcd_put_str("> PIATEK");
		break;
	}
	case 6: {
		lcd_put_str("> SOBOTA");
		break;
	}

	}

}


/************************************************************************************************************************************************************************
 * funkckja mrygacza
 *//********************************************************************************************************************************************************************/

void blink_text(uint8_t x, uint8_t y, uint8_t szer) {
	char bufor_tab[4];
	lcd_locate(y, x);
	char tab[] = { "    " }; //naj³u¿szy bedzie rok czyszczony
	for (uint8_t i = 0; i < szer; i++) {
		bufor_tab[i] = tab[i];
	}
	lcd_locate(y, x);
	lcd_put_str(bufor_tab);
}


