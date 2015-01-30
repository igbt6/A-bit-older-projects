/*
 * ds18b20_obsluga.c
 *
 *  Created on: 2012-02-23
 *       Autor: �ukasz Uszko
 */

#include <avr/io.h>
#include <util/delay.h>
#include "1Wire/ds18x20.h"

#include "ds18b20_obsluga.h"
#include "hd44780.h"
#include "harddef.h"

/*************************************************************************************************************
 * wy�wietlanie temperatury na pozycji X w drugiej linii LCD
 * @param x
 ************************************************************************************************************/

void display_temp(uint8_t x) {
	lcd_cls();

	lcd_locate(0, 0);
	lcd_put_str("> TEMPERATURA:");
	lcd_locate(1, x);
	lcd_put_str("> ");
	if (subzero)
		lcd_put_str("-"); /* je�li subzero==1 wy�wietla znak minus (temp. ujemna) */
	else
		lcd_put_str(" "); /* je�li subzero==0 wy�wietl spacj� zamiast znaku minus (temp. dodatnia) */
	lcd_int(cel); /* wy�wietl dziesi�tne cz�ci temperatury  */
	lcd_put_str("."); /* wy�wietl kropk� */
	lcd_int(cel_fract_bits); /* wy�wietl dziesi�tne cz�ci stopnia */
	lcd_put_str(" C "); /* wy�wietl znak jednostek (C - stopnie Celsiusza) */
}
/********************************************************************************************************
 * funkcja wyszukuj�ca czujnik na magistrali i sprawdzaj�ca czy jest nie uszkodzony np.
 ******************************************************************************************/

void first_search_sensors(void) { /* sprawdzamy ile czujnik�w DS18xxx widocznych jest na magistrali */
	czujniki_cnt = search_sensors();

	/* wysy�amy rozkaz wykonania pomiaru temperatury
	 * do wszystkich czujnik�w na magistrali 1Wire
	 * zak�adaj�c, �e zasilane s� w trybie NORMAL,
	 * gdyby by� to tryb Parasite, nale�a�oby u�y�
	 * jako pierwszego prarametru DS18X20_POWER_PARASITE */
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);

	/* czekamy 800ms na dokonanie konwersji przez pod��czone czujniki */
	//Delay_100ms(8);
	_delay_ms(800);
	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta� wykryty */
	/* wy�wietlamy temperatur� gdy czujnik wykryty */
	/*wyswietlam temperatur� z pierwszego czujnika*/
	if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
			&cel_fract_bits)) {
		return;
	}
	//display_temp(0);
	else {
		lcd_locate(1, 0);
		lcd_put_str("    error ");
		return;//
	}

}

/********************************************************************************************
 * funkcja sprawdzajaca w cyklu odczyt danych z czujnika i wy�wietlajaca go na wyswietlaczu
 *
 ********************************************************************************************/
void read_temp_from_sensors(void) {
	czujniki_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
			&cel_fract_bits)) {
		display_temp(0);
	} else {
		lcd_cls();
		lcd_locate(1, 0);
		lcd_put_str("  error ");
	}

}

/***********************************************************************************************************************************************
 * funkcja sprawdzaj�ca czy zosta�a przekroczona temperatura maksymalna z czujnika 1 , je�li tak to uruchamiane
 * jest wyjscie np wiatrak  i gdy  temp spadnie ponizej temp min to wiatrak si� wy��czy , taka histereza
 * ma�a przydatne jak sterownik pieca, z dok�adnosci� do jednego stopnia
 * @param temp_min ->>>
 * @param temp_max ->>>
 * @param up_or_down)->>> je�li podam UP (logiczna 1) to po osi�gni�ciu temp_max wyj�cie si� w��cza i po opadni�ciu ponizej temp_min  wy��cza si�
 *                   ->>>  -||- -||-  DOWN (logiczne 0) to po spadku poni�ej temp_min uk�ad si� w��cza
 * @return bedzie zwraca� jakie� warto�ci m�wiace ,pewnie dwie o tym w jakim stanie sie znajduje
 ************************************************************************************************************************************************/
bool alarm_from_sensor1(int temp_min, int temp_max, bool up_or_down) {

	static bool histereza;
	//czujniki_cnt = search_sensors();
	//DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	//if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
	//	&cel_fract_bits)) {}
	int temp_zmierzona = 0;
	if (!subzero) {
		temp_zmierzona = cel;
	} else if (subzero) {
		temp_zmierzona = -cel;
	} // je�li zmierzona temp  b�dzie znakiem ujemnym to daj� - przedz stopnieami;

	if (up_or_down) {
		if (histereza == false && temp_zmierzona >= temp_max) {
			DDRD |= 1 << DDD3;
			PORTD |= 1 << PORTD3;
			histereza = true;
			return 1;
		} else if (histereza == true && temp_zmierzona <= temp_min - 1) {
			PORTD &= ~(1 << PORTD3);
			histereza = false;
			return 0;
		}
	} else if (!up_or_down) {
		if (histereza == false && temp_zmierzona <= temp_min-1) {
			DDRD |= 1 << DDD3;
			PORTD |= 1 << PORTD3;
			histereza = true;
			return 1;
		} else if (histereza == true && temp_zmierzona >= temp_max) {
			PORTD &= ~(1 << PORTD3);
			histereza = false;
			return 0;
		}
	}

}

