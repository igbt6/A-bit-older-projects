/*
 * alarms.c
 *
 *  Created on: 2012-03-13
 *       Autor: £ukasz Uszko
 */

#include "alarms.h"
#include <avr/eeprom.h>
#include "harddef.h"
#include "hd44780.h"
#include <avr/io.h>
#include "stdlib.h"

////Alarm_Dobowy *wsk_Alarm_Dobowy = &alarm_dobowy; //definiujê te wskaŸniki
////Alarm_Tygodniowy *wsk_Alarm_Tygodniowy = &alarm_tygodniowy;
////Alarm_Roczny *wsk_Alarm_Roczny = &alarm_roczny;

Alarm_Dobowy ADRES_DOBOWY_W_EEPROM EEMEM;
Alarm_Tygodniowy ADRES_TYGODNIOWY_W_EEPROM EEMEM;
Alarm_Roczny ADRES_ROCZNY_W_EEPROM EEMEM;
/*

 uint8_t EEMEM zmienna1 = 12;
 uint8_t EEMEM zmienna2 = 22;
 uint8_t EEMEM zmienna3 = 32;
 uint8_t EEMEM zmienna4 = 42;
 uint8_t EEMEM zmienna5 = 52;
 uint16_t EEMEM tab_eeprom[] = { 6, 233, 466, 699 };
 uint8_t EEMEM macierz[][4] = { { 0, 0, 0, 0 }, // pierwszy wiersz
 { 0, 0, 0, 0 }, // drugi wiersz
 { 0, 0, 0, 0 } }; //trzeci wiersz
 uint8_t macierz_do_zapisu[][4] = { { 21, 22, 23, 24 }, // pierwszy wiersz
 { 55, 56, 57, 58 }, // drugi wiersz
 { 71, 72, 73, 74 } // trzeci wiersz
 };

 uint8_t eeprom_tablica;
 uint8_t eeprom1, eeprom3, eeprom4, eeprom5;

 //uint8_t (*wsk_w)[4]=macierz ; // wskaŸnik na wiersz tzn. na 5-cio elementow¹ tablicê int
 uint8_t wartosc_eeprom = 99;

 void test_eeprom(void) {

 if (Read_Input_Switch(SWITCH_OK)) {
 lcd_locate(0, 0);
 // eeprom1=eeprom_read_byte(&zmienna1);
 // lcd_str_E()

 eeprom_write_byte(&zmienna1, wartosc_eeprom); //funkcja zapisuj¹ca zmienn¹ wartoœæ  wartosc_eeprom
 eeprom_busy_wait();

 }

 if (Read_Input_Switch(SWITCH_GORA)) {
 lcd_locate(0, 3);

 // eeprom2=eeprom_read_byte(&zmienna2);
 //	 lcd_int(eeprom2);
 eeprom_tablica = eeprom_read_byte(&zmienna3);
 lcd_int(eeprom_tablica);
 }

 if (Read_Input_Switch(SWITCH_LEWO)) {
 lcd_locate(0, 6);
 lcd_int(eeprom3 = eeprom_read_byte(&zmienna3));

 }

 if (Read_Input_Switch(SWITCH_DOL)) {
 lcd_locate(0, 9);
 lcd_int(eeprom4 = eeprom_read_byte(&zmienna1));
 alarm_dobowy.tab_hour[1] = 19;
 alarm_dobowy.tab_hour[2] = 29;
 alarm_dobowy.tab_min[1] = 39;
 alarm_dobowy.tab_min[9] = 49;
 alarm_dobowy.tab_sec[1] = 59;
 alarm_dobowy.tab_sec[3] = 59;
 eeprom_write_block(&alarm_dobowy, &ADRES_DOBOWY_W_EEPROM,
 sizeof(alarm_dobowy)); // zapisujê do pamiêci EEPROM DANE ALARMU

 }
 if (Read_Input_Switch(SWITCH_PRAWO)) {
 lcd_locate(1, 0);
 //lcd_int(eeprom_tablica = eeprom_read_byte(&macierz[2][2]));

 eeprom_read_block(&alarm_dob_test, &ADRES_DOBOWY_W_EEPROM,
 sizeof(alarm_dob_test)); // tutaj czytam ca³¹ strukturê z pamiêci EEPROM

 lcd_int(alarm_dob_test.tab_hour[1]);
 lcd_int(alarm_dob_test.tab_hour[2]);
 lcd_int(alarm_dob_test.tab_min[9]);
 }
 }

 */

/*******************************************************************************************
 * funkcja umo¿liwiaj¹ca ustawienie alarmu ; bêdzie on pracowa³ w 3 trybach
 * 1.dobowy , ustawiam godzinê o której dane urz¹dzenie ma siê w³¹czyæ i po jakim czasie wy³¹czyæ
 * 2.tygodniowy , wybieram w jakich dniach od której do której urz¹dzenie ma sie w³¹czyæ o której wy³¹czyæ // narazie tyle
 * Zrealizujê ja w mikrokontrolerze bo i poco pobierac dane Z RTC jak moger tylko porównywaæ czas z tym z RTC :}
 *//***************************************************************************************/
void set_alarm(uint8_t rodzaj_alarmu) {
	bool flag_next_alarm = 0;
	static uint8_t numer_alarmu = 1;
	uint8_t cursor = 0; // zmienna mówi¹ca o po³ozeniu znacznika przy wyborze alarmu
	//Alarm_Dobowy alarm_dobowy; // tworzê tutaj strukturê na potzreby tego¿ alarmu
	bool flaga_switch = 0; //potrzebne zmienne
	alarm_dobowy.tab_hour[numer_alarmu - 1] = 0; //wyzeruj minuty godziny  i sekundy przed nastaw¹
	alarm_dobowy.tab_min[numer_alarmu - 1] = 0;
	alarm_dobowy.tab_sec[numer_alarmu - 1] = 0;
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
				return;
				flaga_switch = 1;
			}
		}
		switch (rodzaj_alarmu) {
		case 1: {//dobowy
			//while (!flag_next_alarm) {// dopóki nie zmieni siê na jedynkê

			while (!Read_Input_Switch(SWITCH_OK)) {

				show_set_alarm(rodzaj_alarmu, numer_alarmu);
				switch (cursor) {
				case 0: {//GODZINY
					// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ

					if (Read_Input_Switch(SWITCH_GORA)) {
						alarm_dobowy.tab_hour[numer_alarmu - 1]++; // tu mo¿na kiedys wyzmieniaæ ¿eby odnosiæ sie przez wskaŸnik ale narazie nie pali sie i nima po co
						if (alarm_dobowy.tab_hour[numer_alarmu - 1] > 23) {
							alarm_dobowy.tab_hour[numer_alarmu - 1] = 0;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
					} else if (Read_Input_Switch(SWITCH_DOL)) {

						alarm_dobowy.tab_hour[numer_alarmu - 1]--;
						if (alarm_dobowy.tab_hour[numer_alarmu - 1] < 0) {
							alarm_dobowy.tab_hour[numer_alarmu - 1] = 23;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
					}
					if (Read_Input_Switch(SWITCH_PRAWO)) {
						Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
						cursor = 1;

					} else if (Read_Input_Switch(SWITCH_LEWO)) { // jak PRAWO to ustawiasz sekundy
						Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
						cursor = 2;

					}

					break;
				}
				case 1: {//MINUTY
					// jak zaden przycisk nie bêdzie wciœniêty ustawiana wartoœæ bêdzie siê migaæ
					if (Read_Input_Switch(SWITCH_GORA)) {
						alarm_dobowy.tab_min[numer_alarmu - 1]++;
						if (alarm_dobowy.tab_min[numer_alarmu - 1] > 59) {
							alarm_dobowy.tab_min[numer_alarmu - 1] = 0;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
					} else if (Read_Input_Switch(SWITCH_DOL)) {
						//PORTB |= 1 << PB4;
						alarm_dobowy.tab_min[numer_alarmu - 1]--;
						if (alarm_dobowy.tab_min[numer_alarmu - 1] < 0) {
							alarm_dobowy.tab_min[numer_alarmu - 1] = 59;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
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
						alarm_dobowy.tab_sec[numer_alarmu - 1]++;
						if (alarm_dobowy.tab_sec[numer_alarmu - 1] > 59) {
							alarm_dobowy.tab_sec[numer_alarmu - 1] = 0;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
					} else if (Read_Input_Switch(SWITCH_DOL)) {
						alarm_dobowy.tab_sec[numer_alarmu - 1]--;
						if (alarm_dobowy.tab_sec[numer_alarmu - 1] < 0) {
							alarm_dobowy.tab_sec[numer_alarmu - 1] = 59;
						}
						show_set_alarm(rodzaj_alarmu, numer_alarmu);
					}
					if (Read_Input_Switch(SWITCH_LEWO)) { //jak wciœniesz lewy wracasz do minut
						Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
						cursor = 1;
					} else if (Read_Input_Switch(SWITCH_PRAWO)) { // jak PRAWO to ustawiasz sekundy
						Delay_100ms(2); //po to ¿eby nie przeskakiwaæ za daleko po wciœniêciu
						cursor = 0;
					}
					break;
				}
				}
			}
			if (Read_Input_Switch(SWITCH_OK)) {
				Delay_100ms(4);
				lcd_cls();
				lcd_locate(0, 0);
				lcd_str_P(PL_save);
				lcd_locate(1, 0);
				lcd_str_P(PL_next_alarm);
				while (1) { // czeka w nieskoñczonej co poczyni ustawodawca

					if (Read_Input_Switch(SWITCH_LEWO)) {

						numer_alarmu++;
						break; // wróæ z powrotem do pêtli ustawiê alarmu
					} //czy zpisaæ dane? odp: tak
					else if (Read_Input_Switch(SWITCH_PRAWO)) {
                        return;
						break;//czy zpisaæ dane? odp: nie
					} else if (Read_Input_Switch(SWITCH_OK)) { // switch ok nie zapisuj wyjdŸ
						numer_alarmu = 0;
						return;

					}

				}

			}

			//}
		}//tutaj bêdê ustawia³ godziny w jakich ma siê w³¹czaæ i wy³¹czaæ

			}continue;
	}
}

///***************************************************************************************************************************************************
/********************************************************************************************************************************************************
 *
 * //funkcja wyœwietlaj¹ca ustawienia czasu dobowego , 0 ustawiam parametry Startowe , 1 ustawiam parametry Stopowe wy³¹czenia urz¹dzenia
 ********************************************************************************************************************************************************/

void show_set_alarm(uint8_t rodzaj_alarmu, uint8_t numer_parametru/*od 1-10*/) {

	switch (rodzaj_alarmu) {

	case 1: {//dobowy

		{
			lcd_locate(0, 0);
			lcd_put_str("USTAW GODZ. NR:");
			lcd_int(numer_parametru);
			lcd_locate(1, 4);//dobowy
			if (alarm_dobowy.tab_hour[numer_parametru - 1] < 10) {
				lcd_put_str("0");
				lcd_int(alarm_dobowy.tab_hour[numer_parametru - 1]);
			} else {
				lcd_int(alarm_dobowy.tab_hour[numer_parametru - 1]);
			}

			lcd_put_str(":");
			if (alarm_dobowy.tab_min[numer_parametru - 1] < 10) {
				lcd_put_str("0");
				lcd_int(alarm_dobowy.tab_min[numer_parametru - 1]);
			} else {
				lcd_int(alarm_dobowy.tab_min[numer_parametru - 1]);
			}
			lcd_put_str(":");
			if (alarm_dobowy.tab_sec[numer_parametru - 1] < 10) {
				lcd_put_str("0");
				lcd_int(alarm_dobowy.tab_sec[numer_parametru - 1]);
			} else {
				lcd_int(alarm_dobowy.tab_sec[numer_parametru - 1]);
			}

			break;
		}
	}
	case 2: {//tygodniowy
		lcd_locate(0, 0);
		lcd_put_str("USTAW DZIEN NR:");
		lcd_int(numer_parametru);
		lcd_put_str(" ");
		lcd_int(wsk_Alarm_Tygodniowy->tab_day_week[numer_parametru - 1]);
		lcd_locate(1, 3);
		lcd_int(wsk_Alarm_Tygodniowy->tab_hour[numer_parametru - 1]);
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Tygodniowy->tab_min[numer_parametru - 1]);
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Tygodniowy->tab_sec[numer_parametru - 1]);

		break;
	}
	case 3: {//roczny
		lcd_int(wsk_Alarm_Roczny->tab_hour[numer_parametru - 1]); //// TUTAJ DO£OZYC USTAW ROK ........sratatataaaaaaa
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Roczny->tab_min[numer_parametru - 1]);
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Roczny->tab_sec[numer_parametru - 1]);
		lcd_locate(1, 3);
		lcd_int(wsk_Alarm_Roczny->tab_day_month[numer_parametru - 1]);
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Roczny->tab_month[numer_parametru - 1]);
		lcd_put_str("-");
		lcd_int(wsk_Alarm_Roczny->tab_year[numer_parametru - 1]);

		break;
	}

	}

}
