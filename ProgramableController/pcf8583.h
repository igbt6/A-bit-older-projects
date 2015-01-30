/*
 * pcf8583.h
 *
 *  Created on: 2012-02-17
 *       Autor: �ukasz Uszko
 */

#ifndef PCF8583_H_
#define PCF8583_H_

//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
	uint8_t year_to_RTC; // mi�dzy 0 a 3
	int sec_100, sec, min, hour, day_month, month, year, day_week,
			days_in_year, day_of_year;
	int all_days; // zmienna liczy dni a� do przpe�nienia  czyli przez jakies 200 lat potrzeban  mi do rozr�niania dnia tygodnia
} RTC_TIME;
RTC_TIME Rtc_Parametry;

//RTC_TIME*wsk_on_RTC_TIME=&Rtc_Parametry;  // wska�nik na struktur� RTC_TIME
///////////////////////////////////////////////////////////////////////////////////////////
uint8_t dec_to_bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void write_time(uint8_t *tabl_czas);
void write_time_year(RTC_TIME*wsk_on_struct_RTC_TIME);
RTC_TIME* read_time(void);

bool set_time(void);
void show_time(void);
void show_day_week(void);//funkcja wyswietlajaca dzie� tygodnia
//void show_set_time_blink(uint8_t cursor);      //funkcja wyswietlaj�ca zmieniaj�c� si� struktur� Rtc_Parametry z migotaniem
void show_set_time(uint8_t cursor); //funkcja wyswietlaj�ca zmieniaj�c� si� struktur� Rtc_Parametry bez migotania

uint8_t* struct_RTC_on_tab(RTC_TIME* str, uint8_t* tab_czasu); //funkcje konwertujace parametry
RTC_TIME* tab_RTC_on_struct(uint8_t* tab_czas, RTC_TIME *str); //przesy�ane przez wska�niki
// RTC_TIME tab_RTC_on_struct_wart(uint8_t* tab_czasu,  RTC_TIME str);  //funkcje konwertujace parametry przesy�ane przez warto��
//void tab_RTC_on_struct_ref(uint8_t*tab_czasu , RTC_TIME &str ); //przez referencj� nie dzia�a niewiem czemu
void czy_przestepny(RTC_TIME* wsk_struct_RTC_TIME); //funkcja zwraca mi wska�nik na year_to_RTC w zale�no�ci od roku
void dzien_tyg(RTC_TIME *wsk_struct_RTC_TIME); //funkcja licz�ca mi dzie� roku  i ile dni pozosta�o do k��ca
#endif /* PCF8583_H_ */
