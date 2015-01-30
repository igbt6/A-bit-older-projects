/*
 * alarms.h
 *
 *  Created on: 2012-03-13
 *       Autor: £ukasz Uszko
 */

#ifndef ALARMS_H_
#define ALARMS_H_
#define LICZBA_ALARMOW 10

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
/*******************************************************************************************************************************************
 * 3 struktury opisuj¹ce rodzaje alarmów zapisane bêd¹ w EEPROMIE
 ******************************************************************************************************************************************/
typedef struct {
	uint8_t tab_hour[LICZBA_ALARMOW];
	uint8_t tab_min[LICZBA_ALARMOW];
	uint8_t tab_sec[LICZBA_ALARMOW];
} Alarm_Dobowy;

Alarm_Dobowy ADRES_DOBOWY_W_EEPROM ;//tylko deklaracja
// tu rezerwujê sobie Miejsce w EEpromie , i adres
Alarm_Dobowy alarm_dobowy; // tutaj deklaracja struktury któr¹ bêdeê zape³nia³ danymi
Alarm_Dobowy alarm_dob_test;
///////Alarm_Dobowy *wsk_Alarm_Dobowy;  // ustawiam  sobie wskaŸniki na poszczególne strukturki (bedzie ³atwiej operowaæ w ustawieniach)

typedef struct {
	uint8_t tab_hour[LICZBA_ALARMOW];
	uint8_t tab_min[LICZBA_ALARMOW];
	uint8_t tab_sec[LICZBA_ALARMOW];
	uint8_t tab_day_week[LICZBA_ALARMOW];

} Alarm_Tygodniowy;

Alarm_Tygodniowy ADRES_TYGODNIOWY_W_EEPROM ; // tylko deklaracja
// tu rezerwujê sobie Miejsce w EEpromie , i adres
Alarm_Tygodniowy alarm_tygodniowy; // tutaj deklaracja struktury któr¹ bêdeê zape³nia³ danymi
Alarm_Tygodniowy alarm_tyg_test;
Alarm_Tygodniowy *wsk_Alarm_Tygodniowy;  // ustawiam  sobie wskaŸniki na poszczególne strukturki

typedef struct {
	uint8_t tab_hour[LICZBA_ALARMOW];
	uint8_t tab_min[LICZBA_ALARMOW];
	uint8_t tab_sec[LICZBA_ALARMOW];
	uint8_t tab_day_month[LICZBA_ALARMOW];
	uint8_t tab_month[LICZBA_ALARMOW];
	uint16_t tab_year[LICZBA_ALARMOW];

} Alarm_Roczny ;

Alarm_Roczny ADRES_ROCZNY_W_EEPROM; //tylko deklaracja
// tu rezerwujê sobie Miejsce w EEpromie , i adres
 Alarm_Roczny alarm_roczny; // tutaj deklaracja struktury któr¹ bêdeê zape³nia³ danymi
Alarm_Roczny *wsk_Alarm_Roczny;// ustawiam  sobie wskaŸniki na poszczególne strukturki
Alarm_Roczny alarm_year_test;
/******************************************************************************************************************************************/
/********************************************************************************************************************************************
 * DEKLARACJE FUNKCJI ZWIAZANYCH Z ALARMEM
 *******************************************************************************************************************************************/
void test_eeprom(void);
void set_alarm(uint8_t rodzaj_alarmu);
void show_set_alarm(uint8_t rodzaj_alarmu, uint8_t numer_parametru/*od 1-10*/); // t¹ funkcj¹ bêdê wyœwietla³ nastawy alarmu // 1-alarm dobowy//2-alarm tygodniowy//3-alarm roczny

//void show_set_alarm(uint8_t numer_alarmu, bool start_stop_alarm,uint8_t rodzaj_alarmu); //funkcja wyœwietlaj¹ca ustawienia czasu dobowego
//bool set_alarm(void);//funkcja ustawiaj¹ca alarm

#endif /* ALARMS_H_ */
